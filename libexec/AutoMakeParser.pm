#!/usr/bin/perl
#
# SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
# SPDX-License-Identifier: GPL-3.0-or-later
# 
# Copyright (c) 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
# ===
#
# AutoMakeParser.pm
#
# Parser package for parsing automake makefiles, with full variable
# expansion and support for conditional sections.
#
# Synopsis:
#
#  use AutoMakeParser ;
#  $AutoMakeParser::debug = 0 ;
#  my $cs = new ConfigStatus() ; # etc
#  my @makefiles = AutoMakeParser::readall( "." , $cs ) ;
#  my $makefile = new AutoMakeParser( "Makefile.am" , { FOO => 1 , BAR => 0 } , { A => 'aaa' , B => 'bbb' } ) ;
#  $makefile->path() ;
#  $makefile->keys_() ;
#  $makefile->value("some_VAR") ;
#  $makefile->subdirs() ;
#  $makefile->programs() ;
#  $makefile->libraries() ;
#  $makefile->includes() ;
#  $makefile->definitions() ;
#  $makefile->compile_options() ;
#  $makefile->sources('foo') ;
#  $makefile->our_libnames('foo') ;
#  $makefile->our_libdirs('foo') ;
#  $makefile->sys_libs('foo') ;
#
# Two hashes are passed in when parsing: switches and variables. The
# switches are for parsing "if XX/else/endif" directives and the
# variables are used to expand makefile variables like "$(var)".
#
# When reading multiple makefiles with readall() the values of
# the "vars" hash do not change from one makefile to another
# (except when descending into a sub-project -- see
# ConfigStatus.pm). However, variables can be given a value
# that is a perl reference, which allows them to vary through
# the source tree. In practice the "top_srcdir" variable is
# often set as a reference to the up_to_base() function.
#

use strict ;
use IO::File ;
use Cwd ;
use Carp ;
use File::Basename ;

package AutoMakeParser ;
our $debug = 0 ;

sub new
{
	# Reads an automake makefile given by "path", typically named
	# "Makefile.am". The "switches" hash is for parsing "if/else/endif"
	# sections and the "vars_in" hash is for expanding "$(var)"
	# variables. The "depth" and "rdir" parameters are used by readall()
	# for the recursion depth and relative directory.
	#
	# Options:
	#   * strict -- require all variables expanded
	#
	my ( $classname , $path , $switches , $vars_in , $opt , $depth , $rdir ) = @_ ;

	my %me = (
		m_path => simplepath($path) ,
		m_switches => $switches ,
		m_vars => {} ,
		m_depth => $depth , # readall() depth, or undef
		m_rdir => $rdir , # readall() rdir, or undef
		m_lines => [] ,
		m_stack => [] ,
		m_strict => $$opt{strict} ,
	) ;
	my $this = bless \%me , $classname ;
	$this->read( $path ) ;
	$this->parse( $path ) ;
	$this->expand_all( $vars_in ) ;
	$this->copy( $vars_in ) ;
	$this->_log( $opt->{prefix} ) if $opt->{verbose} ;
	return $this ;
}

sub _log
{
	my ( $this , $prefix ) = @_ ;
	for my $k ( sort keys %{$this->{m_vars}} )
	{
		my @v = $this->value( $k ) ;
		print "$prefix$$this{m_path}: $k=[" , join("][",@v) , "]\n" ;
	}
	for my $k ( sort keys %{$this->{m_switches}} )
	{
		my $v = $this->{m_switches}->{$k} ;
		print "$prefix$$this{m_path}: switch: $k=[$v]\n" ;
	}
}

sub path
{
	return $_[0]->{m_path} ;
}

sub readall
{
	# Reads through tree of "Makefile.am" makefiles by following
	# SUBDIRS variables. The "cs" object provides a switches()
	# hash and a vars() hash that might change through the
	# tree.
	#
	# Options:
	#   * verbose -- verbose logging
	#   * prefix -- verbose logging prefix
	#   * strict -- see new()
	#
	my ( $base_dir , $cs , $opt ) = @_ ;

	my $exclude_dir = undef ;
	($base_dir,$exclude_dir) = @$base_dir if ref($base_dir) ;

	# recursive...
	my @makefiles = () ;
	_readall_imp( \@makefiles , 0 , $base_dir , $exclude_dir , "" , $cs , $opt ) ;
	return @makefiles ;
}

sub _readall_imp
{
	my ( $makefiles , $depth , $base_dir , $exclude_dir , $rdir , $cs , $opt ) = @_ ;

	my $base_rdir = _joinpath( $base_dir , $rdir ) ;
	my $path = _joinpath( $base_rdir , "Makefile.am" ) ;
	my $exclude = $exclude_dir && ( $base_rdir =~ m/$exclude_dir/ ) ;
	if( !$exclude )
	{
		print "$$opt{prefix}makefile=[$path] ($depth)\n" if $opt->{verbose} ;
		my $m = new AutoMakeParser( $path , $cs->switchesref($rdir) , $cs->varsref($rdir) , $opt , $depth , $rdir ) ;
		push @$makefiles , $m ;
		for my $subdir ( $m->subdirs() )
		{
			_readall_imp( $makefiles , $depth+1 , $base_dir , $exclude_dir , _joinpath($rdir,$subdir) , $cs , $opt ) ;
		}
	}
}

sub depth
{
	# Returns the readall() recursion depth or undef.
	my ( $this ) = @_ ;
	return $this->{m_depth} ;
}

sub rdir
{
	# Returns the readall() relative directory path
	# (eg. "", "src", or "src/foo") or undef.
	my ( $this ) = @_ ;
	return $this->{m_rdir} ;
}

sub up_to_base
{
	# Returns the relative path up to the first readall()
	# makefile. The returned value will be something like
	# "../../..".
	#
	my ( $this , $adjustment ) = @_ ;
	$adjustment ||= "0" ;
	my $depth = $this->{m_depth} + $adjustment ;
	Carp::confess("bad depth") if ( !defined($depth) || $depth < 0 ) ;
	if( $depth == 0 )
	{
		return "." ;
	}
	else
	{
		( my $base = ( "../" x $depth ) ) =~ s;/$;; ;
		return $base ;
	}
}

sub value
{
	my ( $this , $key ) = @_ ;
	my $v = $this->{m_vars}->{$key} ;
	if( ref($v) eq "CODE" )
	{
		$v = &{$v}($this) ;
	}
	if( !defined($v) || ref($v) )
	{
		return wantarray ? () : undef ;
	}
	else
	{
		return wantarray ? split(' ',$v) : $v ;
	}
}

sub keys_
{
	my ( $this ) = @_ ;
	my @k = sort keys %{$this->{m_vars}} ;
	return wantarray ? @k : join(" ",@k) ;
}

sub programs
{
	# Returns the list of programs derived from the names of all
	# xx_PROGRAMS variables.
	#
	my ( $this ) = @_ ;
	return map { $this->value($_) } grep { m/_PROGRAMS$/ } $this->keys_() ;
}

sub libraries
{
	# Returns the list of libraries derived from the names of all
	# xx_LIBRARIES variables.
	#
	my ( $this ) = @_ ;
	return map { $this->value($_) } grep { m/_LIBRARIES$/ } $this->keys_() ;
}

sub subdirs
{
	# Returns the list of sub-directories derived from the SUBDIRS variable.
	#
	my ( $this ) = @_ ;
	if( !defined($this->{m_vars}->{SUBDIRS}) ) { return () }
	return $this->value( "SUBDIRS" ) ;
}

sub our_libs_raw
{
	# eg. ("../foo/libbar.a",...)
	my ( $this , $program ) = @_ ;
	( my $prefix = $program ) =~ s/[-.]/_/g ;
	return
		grep { my $x = File::Basename::basename($_) ; $x =~ m/^lib.*\.a$/ }
		$this->value( "${prefix}_LDADD" ) ;
}

sub our_libnames
{
	# eg. ("bar",...)
	my ( $this , $program ) = @_ ;
	( my $prefix = $program ) =~ s/[-.]/_/g ;
	return
		map { my $x = File::Basename::basename($_) ; $x =~ s/^lib// ; $x =~ s/\.a$// ; $x }
		grep { my $x = File::Basename::basename($_) ; $x =~ m/^lib.*\.a$/ }
		$this->value( "${prefix}_LDADD" ) ;
}

sub our_libdirs
{
	# Returns directories for "<dir>/lib<name>.a" parts of
	# "<program>_LDADD" taking account of the current
	# makefile's relative directory. The results can be
	# paired with our_libnames().
	#
	# Eg:
	#  src/foo/Makefile.am
	#  => rdir = "src/foo"
	#  => uptobase = "../.."
	#  foo_LDADD = libfoo.a $(top_builddir)/src/bar/libbar.a ../bletch/libbletch.a -Lxxx -lxxx
	#  -> libfoo.a ../../src/bar/libbar.a ../bletch/libbletch.a # value() and grep{}
	#  -> src/foo/libfoo.a src/foo/../../src/bar/libbar.a src/foo/../bletch/libbletch.a # join rdir
	#  -> src/foo src/foo/../../src/bar src/foo/../bletch # dirname()
	#  -> src/foo src/bar src/bletch # simplepath()
	#
	my ( $this , $program ) = @_ ;

	my $up_to_base = $this->up_to_base() || "" ;
	my $rdir = $this->rdir() || "" ;
	( my $program_prefix = $program ) =~ s/[-.]/_/g ;
	return
		map { simplepath( _joinpath($up_to_base,$_) ) }
		map { File::Basename::dirname($_) }
		map { _joinpath($rdir,$_) }
		grep { my $x = File::Basename::basename($_) ; $x =~ m/^lib.*\.a$/ }
		$this->value( "${program_prefix}_LDADD" ) ;
}

sub sys_libs
{
	my ( $this , $program ) = @_ ;
	( my $prefix = $program ) =~ s/[-.]/_/g ;
	my @a =
		map { s/-l// ; $_ }
		grep { m/^-l/ }
		$this->value( "LIBS" ) ;
	my @b =
		map { s/-l// ; $_ }
		grep { m/^-l/ }
		$this->value( "${prefix}_LDADD" ) ;
	return ( @a , @b ) ;
}

sub sources
{
	my ( $this , $target ) = @_ ;
	( my $prefix = $target ) =~ s/[-.]/_/g ;
	return
		grep { m/\.c[p]{0,2}$/ }
		$this->value( "${prefix}_SOURCES" ) ;
}

sub link_options
{
	my ( $this ) = @_ ;
	my @a = $this->value( "AM_LDFLAGS" ) ;
	my @b = $this->value( "LDFLAGS" ) ;
	my @options = ( @a , @b ) ;
	return wantarray ? @options : join(" ",@options) ;
}

sub compile_options
{
	my ( $this ) = @_ ;
	my @a = grep { m/^.std=/ } $this->_compile_options_imp( "CXX" , $this->{m_vars} ) ;
	my @b = $this->_compile_options_imp( "AM_CPPFLAGS" , $this->{m_vars} ) ;
	my @c = $this->_compile_options_imp( "CXXFLAGS" , $this->{m_vars} ) ;
	my @options = ( @a , @b , @c ) ;
	return wantarray ? @options : join(" ",@options) ;
}

sub _compile_options_imp
{
	my ( $this , $var , $vars ) = @_ ;
	$vars ||= $this->{m_vars} ;
	my $s = protect_quoted_spaces( simple_spaces( $vars->{$var} ) ) ;
	$s =~ s/-D /-D/g ;
	$s =~ s/-I /-I/g ;
	return
		map { s/\t/ /g ; $_ }
		grep { !m/-I/ }
		grep { !m/-D/ }
		split( " " , $s ) ;
}

sub definitions
{
	my ( $this ) = @_ ;
	my @a = $this->_definitions_imp( "AM_CPPFLAGS" , $this->{m_vars} ) ;
	my @b = $this->_definitions_imp( "CXXFLAGS" , $this->{m_vars} ) ;
	my @defs = ( @a , @b ) ;
	return wantarray ? @defs : join(" ",@defs) ;
}

sub _definitions_imp
{
	my ( $this , $var , $vars ) = @_ ;
	my $s = protect_quoted_spaces( simple_spaces( $vars->{$var} ) ) ;
	$s =~ s/-D /-D/g ;
	return
		map { s/\t/ /g ; $_ }
		map { s/-D// ; $_ }
		grep { m/-D\S+/ }
		split( " " , $s ) ;
}

sub includes
{
	# Returns a list of include directories derived from the
	# AM_CPPFLAGS and CXXFLAGS variables.
	#
	# All paths are optionally prefixed with the absolute path
	# of this makefile's directory.
	#
	# The returned list also optionally starts with the directory
	# containing the the header file generated by the autoconf
	# "configure" script (eg. "config.h"), using the expansion
	# of "$(top_srcdir)/src".
	#
	my ( $this , $full_paths , $add_autoconf_dir ) = @_ ;
	my $autoconf_dir = exists($this->{m_vars}->{top_srcdir}) ?
		simplepath( _joinpath( $this->value("top_srcdir") , "src" ) ) :
		"" ;
	$autoconf_dir = $this->fullpath( $autoconf_dir ) if $full_paths ;
	my @a = $this->_includes_imp( "AM_CPPFLAGS" , $this->{m_vars} , $full_paths ) ;
	my @b = $this->_includes_imp( "CXXFLAGS" , $this->{m_vars} , $full_paths ) ;
	my @c = ( $autoconf_dir && $add_autoconf_dir ) ? ( $autoconf_dir ) : () ;
	my @incs = ( @c , @a , @b ) ;
	return wantarray ? @incs : join(" ",@incs) ;
}

sub _includes_imp
{
	my ( $this , $var , $vars , $full_paths ) = @_ ;
	my $s = protect_quoted_spaces( simple_spaces( $vars->{$var} ) ) ;
	$s =~ s/-I /-I/g ;
	return
		map { $full_paths?$this->fullpath($_):$_ }
		map { simplepath($_) }
		map { s/\t/ /g ; $_ }
		map { s:-I:: ; $_ } grep { m/-I\S+/ }
		split( " " , $s ) ;
}

# --

sub vars
{
	return $_[0]->{m_vars}
}

sub _joinpath
{
	# Returns a joined path, removing empty parts.
	my ( @args ) = @_ ;
	return join( "/" , grep {/./} @args ) ;
}

sub simplepath
{
	# Returns a simplified path by removing "." and "xx/.." parts.
	my ( $path ) = @_ ;
	my $first = ( $path =~ m;^/; ) ? "/" : "" ;
	my @out = () ;
	my @split = grep {m/./} split( "/" , $path ) ;
	for my $x ( @split )
	{
		next if( $x eq "" || $x eq "." ) ;
		if( $x eq ".." && scalar(@out) && $out[-1] ne ".." )
		{
			pop @out ;
		}
		else
		{
			push @out , $x ;
		}
	}
	return $first . join( "/" , @out ) ;
}

sub fullpath
{
	# Joins the absolute path of this makefile's directory to the
	# given relative path.
	#
	my ( $this , $relpath ) = @_ ;
	my $this_makefile = Cwd::realpath( $this->path() ) ;
	my $this_dir = File::Basename::dirname( $this_makefile ) ;
	return simplepath( _joinpath($this_dir,$relpath) ) ;
}

sub simple_spaces
{
	my ( $s ) = @_ ;
	$s =~ s/\s/ /g ;
	$s =~ s/^ *// ;
	$s =~ s/ *$// ;
	return $s ;
}

sub protect_quoted_spaces
{
	my ( $s , $tab ) = @_ ;
	if( $s =~ m/"/ )
	{
		my @x = split( /"/ , $s ) ;
		if( $s =~ m/"$/ ) { push @x , "" }
		for( my $i = 0 ; $i < scalar(@x) ; $i++ )
		{
			if( ($i%2) == 1 ) { $x[$i] =~ s/ /$tab/g }
		}
		$s = join( '"' , @x ) ;
	}
	return $s ;
}

sub size
{
	my ( $this ) = @_ ;
	return scalar(@{$this->{m_lines}}) ;
}

sub empty
{
	my ( $this ) = @_ ;
	return $this->size() == 0 ;
}

sub lastline
{
	my ( $this ) = @_ ;
	return ${$this->{m_lines}}[-1] ;
}

sub continued
{
	my ( $this ) = @_ ;
	return !$this->empty() && $this->lastline() =~ m/\\\s*$/ ;
}

sub continue
{
	my ( $this , $line ) = @_ ;
	${$this->{m_lines}}[-1] =~ s/\\\s*$// ;
	${$this->{m_lines}}[-1] .= $line ;
}

sub add
{
	my ( $this , $n , $line ) = @_ ;
	while( $this->size() < ($n-3) ) { push @{$this->{m_lines}} , "" }
	push @{$this->{m_lines}} , $line ;
}

sub copy
{
	my ( $this , $vars_in ) = @_ ;
	return if !defined($vars_in) ;
	for my $k ( keys %$vars_in )
	{
		if( !exists($this->{m_vars}->{$k}) )
		{
			$this->{m_vars}->{$k} = $vars_in->{$k} ;
		}
	}
}

sub read
{
	my ( $this , $path ) = @_ ;
	my $fh = new IO::File( $path ) or die "error: cannot open automake file: [$path]\n" ;
	my $n = 1 ;
	while(<$fh>)
	{
		chomp( my $line = $_ ) ;
		$this->continued() ? $this->continue($line) : $this->add($n,$line) ;
		$n++ ;
	}
}

sub parse
{
	my ( $this ) = @_ ;
	my @handlers = (
		[ qr/^\s*$/ , \&AutoMakeParser::do_blank ] ,
		[ qr/^\s*#/ , \&AutoMakeParser::do_comment ] ,
		[ qr/^\s*(\S+)\s*\+=\s*(.*)/ , \&AutoMakeParser::do_assign_more ] ,
		[ qr/^\s*(\S+)\s*(=|\?=|:=|::=)\s*(.*)/ , \&AutoMakeParser::do_assign ] ,
		[ qr/^\s*if\s+(\S+)/ , \&AutoMakeParser::do_if ] ,
		[ qr/^\s*else\s*$/ , \&AutoMakeParser::do_else ] ,
		[ qr/^\s*endif\s*$/ , \&AutoMakeParser::do_endif ] ,
	) ;
	my $n = 0 ;
	for my $line ( @{$this->{m_lines}} )
	{
		$n++ ;
		for my $h ( @handlers )
		{
			my ( $hre , $hfn ) = @$h ;
			if( $line =~ $hre )
			{
				&{$hfn}( $this , $n , $line , $1 , $2 , $3 , $4 , $5 , $6 ) ;
				last ; # (new)
			}
		}
		debug_( "$$this{m_path}($n): " , $line ) if ( $this->enabled() && ( $line =~ m/\S/ ) && ( $line !~ m/^#/ ) ) ;
	}
	for my $k ( sort keys %{$this->{m_vars}} )
	{
		my $v = $this->{m_vars}->{$k} ;
		debug_( "$$this{m_path}: var: [$k] = [$v]" ) ;
	}
}

sub enabled
{
	my ( $this ) = @_ ;
	my $all = scalar( @{$this->{m_stack}} ) ;
	my $on = scalar( grep { $_ == 1 } @{$this->{m_stack}} ) ;
	return $all == $on ;
}

sub expand_all
{
	my ( $this , $ro_vars ) = @_ ;
	for my $k ( sort keys %{$this->{m_vars}} )
	{
		$this->expand( $k , $ro_vars ) ;
	}
}

sub expand
{
	my ( $this , $k , $ro_vars ) = @_ ;
	my $v = $this->{m_vars}->{$k} ;
	my $vv = $this->expansion( $v , $ro_vars , $k ) ;
	if( $v ne $vv )
	{
		debug_( "$$this{m_path}: expansion: [$k]..." ) ;
		debug_( "$$this{m_path}: expansion:   [$v]" ) ;
		debug_( "$$this{m_path}: expansion:   [$vv]" ) ;
	}
	$this->{m_vars}->{$k} = $vv ;
}

sub expansion
{
	my ( $this , $v , $ro_vars , $context ) = @_ ;
	my $strict = $this->{m_strict} ;
	while(1)
	{
		my ( $kk ) = ( $v =~ m/\$\(([^)]+)\)/ ) ;
		my $pre = $` ;
		my $post = $' ;
		return $v if !defined($kk) ;

		my $vv = undef ;
		if( exists($this->{m_vars}->{$kk}) )
		{
			$vv = $this->{m_vars}->{$kk} ;
		}
		elsif( exists($ro_vars->{$kk}) )
		{
			if( ref($ro_vars->{$kk}) eq "CODE" )
			{
				$vv = &{$ro_vars->{$kk}}( $this ) ;
				die "error: $$this{m_path}: $context: invalid expansion of [$kk] in [$v]\n" if !scalar($vv) ;
			}
			else
			{
				$vv = $ro_vars->{$kk} ;
			}
		}

		my $ok = defined($vv) && !ref($vv) ;
		if( $ok )
		{
			$v = $pre . $vv . $post ;
		}
		elsif( $strict )
		{
			die "error: $$this{m_path}: $context: no value for expansion of [$kk] in [$v]\n" ;
		}
		else
		{
			$v = $pre . "$(" . $vv . ")" . $post ; # leave it unexpanded
		}
	}
}

sub do_blank
{
	my ( $this , $n , $line ) = @_ ;
}

sub do_comment
{
	my ( $this , $n , $line ) = @_ ;
}

sub do_if
{
	my ( $this , $n , $line , $switch ) = @_ ;
	my $value = ( exists $this->{m_switches}->{$switch} && $this->{m_switches}->{$switch} ) ? 1 : 0 ;
	push @{$this->{m_stack}} , $value ;
}

sub do_else
{
	my ( $this , $n , $line ) = @_ ;
	die if scalar(@{$this->{m_stack}}) == 0 ;
	${$this->{m_stack}}[-1] = ${$this->{m_stack}}[-1] == 1 ? 0 : 1 ;
}

sub do_endif
{
	my ( $this , $n , $line ) = @_ ;
	pop @{$this->{m_stack}} ;
}

sub do_assign
{
	my ( $this , $n , $line , $lhs , $eq , $rhs ) = @_ ;
	if( $this->enabled() )
	{
		$rhs =~ s/\s+/ /g ;
		$rhs =~ s/^\s*// ;
		$rhs =~ s/\s*$// ;
		# TODO if $eq is "?="
		$this->{m_vars}->{$lhs} = $rhs ;
	}
}

sub do_assign_more
{
	my ( $this , $n , $line , $lhs , $rhs ) = @_ ;
	if( $this->enabled() )
	{
		$rhs =~ s/\s+/ /g ;
		$rhs =~ s/^\s*// ;
		$rhs =~ s/\s*$// ;
		$this->{m_vars}->{$lhs} = join( " " , $this->{m_vars}->{$lhs} , $rhs ) ;
	}
}

sub debug_
{
	my $line = join( " " , @_ ) ;
	$line =~ s/ *\t */ /g ;
	print $line , "\n" if $debug ;
}

1 ;
