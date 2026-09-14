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
# CompilationDatabase.pm
#
# Parses automake files throughout a source tree using AutoMakeParser
# and generates a "compilation database" file, "compile_commands.json".
#
# See also: https://clang.llvm.org/docs/JSONCompilationDatabase.html
#
# Synopsis:
#
#    use CompilationDatabase ;
#    my @makefiles = AutoMakeParser::readall( ... ) ;
#    my $cs = new ConfigStatus() ; # etc
#    my $cdb = new CompilationDatabase( \@makefiles , {full_paths=>1} ) ;
#    my $cdb = new CompilationDatabase( $src_dir , $cs , {full_paths=>1} ) ;
#    my @files = $cdb->list() ;
#    my @stanzas = $cdb->stanzas() ;
#    $cdb->print() ;
#

use strict ;
use File::Basename ;
use AutoMakeParser ;

package CompilationDatabase ;
our $debug = 0 ;

sub new
{
	if( ref($_[1]) ) # eg. \@makefiles
	{
		# Parses a set of makefiles as given by an array of Makefile
		# objects obtained from AutoMakeParser::readall().
		#
		my ( $classname , $makefiles_ref , $config ) = @_ ;
		$config ||= {} ;
		$config->{test_mode} ||= 0 ;
		$config->{full_paths} ||= 0 ;
		my %me = (
			m_makefiles => $makefiles_ref ,
			m_config => $config ,
		) ;
		return bless \%me , $classname ;
	}
	else # eg. $src_dir
	{
		# Finds makefiles under the given base directory and parses
		# them. The switches and read-only expansion variables are
		# provided via the 'cs' parameter (see ConfigStatus) which
		# can be parsed out of "config.status" files or load()ed
		# in from elsewhere.
		#
		my ( $classname , $base_makefile_dir , $cs , $config ) = @_ ;
		die if ref($cs) ne "ConfigStatus" ;
		$config ||= {} ;
		$config->{test_mode} ||= 0 ;
		$config->{full_paths} ||= 0 ;
		$AutoMakeParser::debug = 1 if $debug > 1 ;
		my @makefiles = AutoMakeParser::readall( $base_makefile_dir , $cs , {strict=>1} ) ;
		my %me = (
			m_makefiles => \@makefiles ,
			m_config => $config ,
		) ;
		return bless \%me , $classname ;
	}
}

sub list
{
	# Returns a list of all the source files in all the makefiles found under base-dir.
	my ( $this ) = @_ ;

	my @list = () ;
	my $verbose = $debug ;
	for my $m ( @{$this->{m_makefiles}} )
	{
		my $sub_dir = File::Basename::dirname( $m->path() ) ;
		for my $library ( $m->libraries() )
		{
			push @list , map { "$sub_dir/$_" } $m->sources($library) ;
		}
		for my $program ( $m->programs() )
		{
			push @list , map { "$sub_dir/$_" } $m->sources($program) ;
		}
	}
	return @list ;
}

sub print
{
	# Prints the complete compilation database json structure to stdout.
	my ( $this ) = @_ ;
	print "[\n" ;
	print join( ",\n" , $this->stanzas() ) ;
	print "]\n" ;
}

sub stanzas
{
	# Returns a list of separate compilation database stanzas for all the source files
	# in all the makefiles found under base-dir.
	my ( $this ) = @_ ;

	my @output = () ;
	for my $m ( @{$this->{m_makefiles}} )
	{
		my $dir = File::Basename::dirname( $m->path() ) ;
		my @includes = map { "-I$_" } $m->includes( $this->{m_config}->{full_paths} , 1 ) ;
		my @definitions = map { "-D$_" } $m->definitions() ;
		my @compile_options = $m->compile_options() ;
		my @link_options = $m->link_options() ;

		if( $debug )
		{
			print "cdb: makefile=" , $m->path() , "\n" ;
			print "cdb:  base=",$m->base(),"\n" ;
			print "cdb:  \@includes=" , join("|",@includes) , "\n" ;
			print "cdb:  \@definitions=" , join("|",@definitions) , "\n" ;
			print "cdb:  \@compile_options=" , join("|",@compile_options) , "\n" ;
			print "cdb:  \@link_options=" , join("|",@link_options) , "\n" ;
		}

		for my $library ( $m->libraries() )
		{
			map { push @output , $this->_stanza($dir,$_,\@includes,\@definitions,\@compile_options) } $m->sources($library) ;
		}
		for my $program ( $m->programs() )
		{
			map { push @output , $this->_stanza($dir,$_,\@includes,\@definitions,\@compile_options) } $m->sources($program) ;
		}
	}
	return @output ;
}

sub _stanza
{
	my ( $this , $dir , $source , $includes , $definitions , $options ) = @_ ;

	my @all_flags = ( @$definitions , @$includes , @$options ) ;

	my $directory = Cwd::realpath( $dir ) ;
	$source = join("/",$directory,$source) if $this->{m_config}->{full_paths} ;

	my $command = "/usr/bin/c++ @all_flags -c $source" ;
	$command =~ s/"/\\"/g unless $this->{m_config}->{test_mode} ;

	if( $this->{m_config}->{test_mode} )
	{
		return "cd $directory && $command\n" ;
	}

	return
		"{\n" .
		" \"directory\" : \"$directory\" ,\n" .
		" \"command\" : \"$command\",\n" .
		" \"file\" : \"$source\" ,\n" .
		"}" ;
}

1 ;
