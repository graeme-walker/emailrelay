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
# winbuild.pm
#
# Helper functions for winbuild.pl.
#
# Synopsis:
#
#  require "winbuild.pm" ;
#  winbuild::default_touchfile(...) ;
#  winbuild::find_cmake(...) ;
#  winbuild::find_qt_x86(...) ;
#  winbuild::find_qt_x64(...) ;
#  winbuild::find_mbedtls_src(...) ;
#  winbuild::spit_out_batch_files(...) ;
#  winbuild::clean_cmake_files(...) ;
#  winbuild::clean_batch_files(...) ;
#  winbuild::deltree(...) ;
#  winbuild::create_touchfile(...) ;
#  winbuild::touch(...) ;
#

use strict ;
use Cwd ;
use IO::File ;
use File::Basename ;
use File::Find ;
use File::Path ;
use File::Glob ;
use File::Copy ;

package winbuild ;

sub _find_cmake
{
	my $ms = "Microsoft Visual Studio/*/*/Common7/IDE/CommonExtensions/Microsoft/CMake/" ;
	return (
		_find_basic( "find-cmake" , "cmake.exe" , grep { m/cmake/ } _path_dirs() ) ||
		_find_match( "find-cmake" , "cmake*/bin/cmake.exe" , undef , $ENV{ProgramFiles}."/$ms" ) ||
		_find_match( "find-cmake" , "cmake*/bin/cmake.exe" , undef , $ENV{'ProgramFiles(x86)'}."/$ms" ) ||
		_find_basic( "find-cmake" , "cmake.exe" , grep { !m/cmake/ } _path_dirs() ) ||
		_find_match( "find-cmake" , "cmake*/bin/cmake.exe" , "$ENV{SystemDrive}" ) ) ;
}

sub find_cmake
{
	# Finds cmake.
	return ( -x "/usr/bin/cmake" ? "/usr/bin/cmake" : undef ) if $^O eq "linux" ;
	return _find_cmake() ;
}

sub find_qt_x86
{
	# Finds a Qt binary release for x86 in the Qt installer's standard location.
	my $up_dir = File::Basename::dirname( Cwd::realpath(File::Basename::dirname($0)) ) ;
	my $up_up_dir = File::Basename::dirname( $up_dir ) ;
	my $qt = _find_basic( "find-qt(x86)" , "qt-bin-x86" , $up_dir , $up_up_dir ) ; # see qtbuild.pl
	return $qt if $qt ;
	my @dirs = ( "$ENV{HOMEDRIVE}$ENV{HOMEPATH}/qt" , "$ENV{SystemDrive}/qt" ) ;
	my $qt_include =
		_find_match( "find-qt(x86)" , "5*/msvc*/include/" , qr;/msvc\d\d\d\d/; , @dirs ) ||
		_find_match( "find-qt(x86)" , "6*/msvc*/include/" , qr;/msvc\d\d\d\d/; , @dirs ) ;
	return undef if !$qt_include ;
	( $qt = $qt_include ) =~ s;/include$;; ;
	return $qt ;
}

sub find_qt_x64
{
	# Finds a Qt binary release for x64 in the Qt installer's standard location.
	my $up_dir = File::Basename::dirname( Cwd::realpath(File::Basename::dirname($0)) ) ;
	my $up_up_dir = File::Basename::dirname( $up_dir ) ;
	my $qt = _find_basic( "find-qt(x64)" , "qt-bin-x64" , $up_dir , $up_up_dir ) ; # see qtbuild.pl
	return $qt if $qt ;
	my @dirs = ( "$ENV{HOMEDRIVE}$ENV{HOMEPATH}/qt" , "$ENV{SystemDrive}/qt" ) ;
	my $qt_include =
		_find_match( "find-qt(x64)" , "5*/msvc*_64/include/" , undef , @dirs ) ||
		_find_match( "find-qt(x64)" , "6*/msvc*_64/include/" , undef , @dirs ) ;
	return undef if !$qt_include ;
	( $qt = $qt_include ) =~ s;/include$;; ;
	return $qt ;
}

sub _find_mbedtls_src
{
	my $version_c =
		_find_match( "find-mbedtls-src" , "mbedtls*/library/version.c" , undef ,
			File::Basename::dirname($0) ,
			File::Basename::dirname($0)."/.." ) ;
	return -f $version_c ? File::Basename::dirname(File::Basename::dirname($version_c)) : undef ;
}

sub find_mbedtls_src
{
	# Finds a mbedtls source tree in "." or "..".
	return ( -d "mbedtls" ? "mbedtls" : undef ) if $^O eq "linux" ;
	return _find_mbedtls_src() ;
}

sub _find_basic
{
	my ( $logname , $fname , @dirs ) = @_ ;
	my $result ;
	for my $dir ( map {_sanepath($_)} @dirs )
	{
		my $path = "$dir/$fname" ;
		if( -e $path ) { $result = Cwd::realpath($path) ; last }
		print "$logname: not $path\n" ;
	}
	print "$logname=[$result]\n" if $result ;
	return $result ;
}

sub _find_under
{
	my ( $logname , $fname , @dirs ) = @_ ;
	my $result ;
	for my $dir ( map {_sanepath($_)} @dirs )
	{
		next if !$dir ;
		my @find_list = () ;
		File::Find::find( sub { push @find_list , $File::Find::name if lc($_) eq $fname } , $dir ) ;
		if( @find_list ) { $result = Cwd::realpath($find_list[0]) ; last }
		print "$logname: not under $dir\n" ;
	}
	print "$logname=[$result]\n" if $result ;
	return $result ;
}

sub _find_match
{
	my ( $logname , $glob , $re , @dirs ) = @_ ;
	$re = qr;.; if !defined($re) ;

	my $find_dir = ( $glob =~ m;/$; ) ;
	$glob =~ s;/$;; if $find_dir ;

	# for each base directory in @dirs do the file glob and return
	# the first glob path that matches the regex -- if the glob
	# pattern ends in a slash then only directories are returned
	# from the glob stage

	my $result ;
	for my $dir ( map {_sanepath($_)} @dirs )
	{
		my @glob_match = () ;
		push @glob_match , grep { -e $_ && $_ =~ m/$re/ && ( !$find_dir || -d $_ ) } File::Glob::bsd_glob( "$dir/$glob" ) ;
		if( @glob_match ) { $result = Cwd::realpath($glob_match[0]) ; last }
		print "$logname: no match for $dir/$glob\n" ;
	}
	print "$logname=[$result]\n" if( $result && $logname ) ;
	return $result ;
}

sub default_touchfile
{
	my ( $script ) = @_ ;
	$script =~ s/\.pl$// ;
	return "$script.ok" ;
}

sub create_touchfile
{
	my ( $touchfile ) = @_ ;
	my $fh = new IO::File( $touchfile , "w" ) or die ;
	$fh->close() or die ;
}

sub spit_out_batch_files
{
	my ( @parts ) = @_ ;
	for my $part ( @parts )
	{
		my $fname = "winbuild-$part.bat" ;
		if( ! -f $fname )
		{
			my $fh = new IO::File( $fname , "w" ) or next ;
			print $fh "runperl winbuild.pl winbuild.ok $part\n" ;
			$fh->close() ;
		}
	}
}

sub clean_cmake_files
{
	my ( $base_dir ) = @_ ;
	$base_dir ||= "." ;
	my @list = () ;
	File::Find::find( sub { push @list , $File::Find::name if $_ eq "CMakeLists.txt" } , $base_dir ) ;
	unlink grep {!m/mbedtls/i} grep {!m/qt/i} @list ;
}

sub clean_batch_files
{
	my ( $base_dir ) = @_ ;
	$base_dir ||= "." ;
	my @list = File::Glob::bsd_glob( "$base_dir/winbuild-*.bat" ) ;
	unlink @list ;
}

sub deltree
{
	my ( $dir ) = @_ ;
	my $e ;
	File::Path::remove_tree( $dir , {safe=>1,verbose=>0,error=>\$e} ) ;
	if( $e && scalar(@$e) )
	{
		for my $x ( @$e )
		{
			my ( $f , $m ) = ( %$x ) ;
			print "warning: " . ($f?"[$f]: ":"") . $m , "\n" ;
		}
	}
}

sub touch
{
	my ( $path ) = @_ ;
	if( ! -f $path )
	{
		my $fh = new IO::File( $path , "w" ) or die ;
		$fh->close() or die ;
	}
}

sub _path_dirs
{
	my $path = $ENV{PATH} ;
	my $sep = ( $path =~ m/;/ ) ? ";" : ":" ;
	return split( $sep , $path ) ;
}

sub _sanepath
{
	my ( $path ) = @_ ;
	$path =~ s:\\:/:g ;
	return $path ;
}

package main ;

1 ;
