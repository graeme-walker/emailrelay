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
# Doxyfile.pm
#
# Generates a doxygen configuration file by running "doxygen -g".
#
# Synopsis:
# use Doxyfile ;
# my $df = new Doxyfile( {
#	COLOUR => "..." , # (overrides HTML_COLORSTYLE_*)
#	SRC => "..." , # (overrides INPUT)
#	PROJECT_NAME => "..." ,
#	PROJECT_LOGO => "..." ,
#	INPUT => "..." ,
#	HTML_OUTPUT => "..." ,
#	FILE_PATTERNS => [ "..." ] ,
#	EXCLUDE_PATTERNS => [ "..." ] ,
#	EXCLUDE_SYMBOLS => [ "..." ] ,
#	HTML_HEADER => "..." ,
#	HTML_FOOTER => "..." ,
#	HTML_EXTRA_STYLESHEET => [ "..." ] ,
#	HTML_COLORSTYLE_HUE => "..." ,
#	HTML_COLORSTYLE_SAT => "..." ,
#	HTML_COLORSTYLE_GAMMA => "..." ,
#	DISABLE_INDEX => "YES" ,
#	GENERATE_TREEVIEW => "YES" ,
#	TOC_INCLUDE_HEADINGS => 3 , # for markdown
#	SHOW_FILES => "YES" ,
#	SEARCHENGINE => "..." ,
#	LAYOUT_FILE => "..." ,
#	... ) ;
# $df->run() ;
# $df->purge() ;
# $df->substitute( '@FOO@" => "foo" , '@BAR@' => "bar" ) ;
# my @lines = $df->lines() ;
# $df->write( "doxyfile" ) ;
#

use strict ;
use IO::File ;
use File::Basename ;
use Data::Dumper ;
use Carp ;

package Doxyfile ;

my %colours = (
	# hue(/255), saturation(/255), gamma(/255)
	pink => [ 260 , 100 , 80 ] , # #573d8c ~= #649
	copper => [ 15 , 100 , 60 ] ,
	blue => [ 195 , 255 , 100 ] , # (195/255,255/255,38/100) is #0090c0
) ;

# default configuration items -- scalar or array-ref items -- undefined values must be passed in
my %defaults = (
	PROJECT_NAME => undef ,
	PROJECT_LOGO => undef ,
	INPUT => undef ,
	HTML_OUTPUT => undef ,
	FILE_PATTERNS => [ undef ] ,
	EXCLUDE_PATTERNS => [ undef ] ,
	EXCLUDE_SYMBOLS => [ undef ] ,
	GENERATE_LATEX => "NO" ,
	JAVADOC_AUTOBRIEF => "YES" ,
	MULTILINE_CPP_IS_BRIEF => "NO" ,
	BUILTIN_STL_SUPPORT => "YES" ,
	GENERATE_HTML => "YES" ,
	'GENERATE_TO'.'DOLIST' => "NO" ,
	GENERATE_TESTLIST => "NO" ,
	GENERATE_BUGLIST => "NO" ,
	GENERATE_DEPRECATEDLIST => "NO" ,
	##SHOW_NAMESPACES => "NO" , # moot
	QUIET => "YES" ,
	WARNINGS => "NO" ,
	RECURSIVE => "YES" , # moot
	##USE_MDFILE_AS_MAINPAGE => "..." ,
	SOURCE_BROWSER => "YES" ,
	STRIP_CODE_COMMENTS => "NO" ,
	HTML_HEADER => undef ,
	HTML_FOOTER => undef ,
	HTML_EXTRA_STYLESHEET => [ undef ] ,
	HTML_DYNAMIC_SECTIONS => "YES" ,
	HTML_TIMESTAMP => "NO" ,
	GENERATE_TREEVIEW => "YES" ,
	TREEVIEW_WIDTH => "100" ,
	MACRO_EXPANSION => "YES" ,
	EXPAND_ONLY_PREDEF => "YES" ,
	PREDEFINED => join( " " , qw(
		G_DOXYGEN=1
		G_UNIX=1
		GCONFIG_ENABLE_STD_THREAD=1
		__cplusplus=201400
		G_EXCEPTION()=
		G_EXCEPTION_CLASS()=
		__declspec__(x)=
		__attribute__(x)=
	) ) ,
	HTML_COLORSTYLE_HUE => undef ,
	HTML_COLORSTYLE_SAT => undef ,
	HTML_COLORSTYLE_GAMMA => undef ,
	HTML_TIMESTAMP => "NO" ,
	DISABLE_INDEX => undef ,
	GENERATE_TREEVIEW => undef ,
	MARKDOWN_SUPPORT => "YES" ,
	TOC_INCLUDE_HEADINGS => undef , # for markdown
	SHOW_FILES => undef ,
	SEARCHENGINE => undef ,
	LAYOUT_FILE => undef ,
) ;

sub new
{
	my ( $classname , $opt_in ) = @_ ;
	ref($opt_in) eq "HASH" or die ;

	my %opt = %defaults ;
	for my $key ( keys %$opt_in )
	{
		if( ( $key eq "COLOUR" ) || ( $key eq "SRC" ) )
		{
		}
		else
		{
			_die("invalid item [$key]") if( !exists($opt{$key}) ) ;
			_die("invalid item type [$key]") if( ref($opt{$key}) ne ref($opt_in->{$key}) ) ;
			$opt{$key} = $opt_in->{$key} ;
		}
	}

	# use 'src' to override 'input'
	if( exists $opt_in->{SRC} )
	{
		_die("SRC overrides INPUT") if defined($opt{INPUT}) ;
		$opt{INPUT} = join(" ",map {"__TOP_SRC__/src/$_"} split(/,/,$opt_in->{SRC}) ) ;
		delete $opt_in->{SRC} ;
	}

	# use 'colour' to override 'hue/saturation/gamma'
	if( exists $opt_in->{COLOUR} )
	{
		my $colour = $opt_in->{COLOUR} ;
		_die("invalid colour [$colour]") if !exists $colours{$colour} ;
		my @hsv = @{$colours{$colour}} ;
		$opt{HTML_COLORSTYLE_HUE} = $hsv[0] ;
		$opt{HTML_COLORSTYLE_SAT} = $hsv[1] ;
		$opt{HTML_COLORSTYLE_GAMMA} = $hsv[2] ;
		delete $opt_in->{COLOUR} ;
	}

	for my $key ( keys %opt )
	{
		_die("missing scalar item [$key]") if( !defined($opt{$key}) ) ;
		_die("missing list item [$key]") if( ref($opt{$key}) && !defined(@{$opt{$key}}[0]) ) ;
	}

	return bless {
		m_opt => \%opt ,
		m_lines => [] ,
	} , $classname ;
}

sub dump
{
	my ( $this ) = @_ ;
	local $Data::Dumper::Sortkeys = 1 ;
	print Data::Dumper::Dumper( $this->{m_opt} ) ;
}

sub run
{
	my ( $this , $cmd ) = @_ ;
	$cmd ||= "doxygen -g -" ;
	my $opt = $this->{m_opt} ;

	my $fh = new IO::File( "$cmd |" ) or _die("no doxygen") ;
	my $discard_continuation = 0 ;
	while(<$fh>)
	{
		chomp( my $line = $_ ) ;
		if( $discard_continuation )
		{
			$discard_continuation = ( $line =~ m/\\$/ ) ;
			next ;
		}
		for my $key ( keys %$opt ) # O(NxM) :-<
		{
			if( $line =~ m/^$key *=/ )
			{
				$discard_continuation = ( $line =~ m/\\$/ ) ;
				if( ref($opt->{$key}) )
				{
					$line = "$key = " . join(" ",@{$opt->{$key}}) ;
				}
				else
				{
					$line = "$key = " . $opt->{$key} ;
				}
				last ;
			}
		}
		push @{$this->{m_lines}} , $line ;
	}
}

sub purge
{
	my ( $this ) = @_ ;
	my @new_lines = grep {m/^[^#]/} @{$this->{m_lines}} ;
	$this->{m_lines} = \@new_lines ;
	_die("empty") if $this->empty() ;
}

sub substitute
{
	my ( $this , $map ) = @_ ;
	_die("empty") if $this->empty() ;
	for my $key ( keys %$map )
	{
		my $value = $map->{$key} ;
		for my $i ( 0 .. (scalar(@{$this->{m_lines}})-1) )
		{
			@{$this->{m_lines}}[$i] =~ s/\Q$key\E/$value/g ;
		}
	}
}

sub empty
{
	my ( $this ) = @_ ;
	return scalar(@{$this->{m_lines}}) == 0 ;
}

sub lines
{
	my ( $this ) = @_ ;
	_die("empty") if $this->empty() ;
	return @{$this->{m_lines}} ;
}

sub write
{
	my ( $this , $path ) = @_ ;
	_die("empty") if $this->empty() ;
	my $fh = new IO::File( $path , "w" ) or _die() ;
	for my $line ( @{$this->{m_lines}} )
	{
		print $fh $line , "\n" ;
	}
	$fh->close() or _die() ;
}

sub _die
{
	Carp::confess "error: Doxyfile.pm".join(" ",(": ",@_))."\n" ;
}

1 ;

