#!/usr/bin/env perl
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
# libresslbuild.pl
#
# Builds libressl on Windows using cmake, with "/MT" used for
# a statically-linked run-time library.
#
# usage: libresslbuild.pl [<options>] [<src-dir> [<build-dir>]]
#         --arch={x64|x86}            build architecture
#         --config={debug|release}    build type
#         --cmake=<path>              cmake
#         --makefile=<path>           nmake makefile if no cmake
#         <src-dir>                   source directory
#         <build-dir>                 build directory
#
# The command-line directories default as follows:
#     src-dir    - dirname($0)/libressl
#     build-dir  - <cwd>/libressl-<arch>
#
# Libraries and headers end up in:
#     libs    - <build-dir>/library/<config>
#     headers - <build-dir>/include/openssl
#

use strict ;
use Getopt::Long ;
use File::Basename ;
use File::Copy ;
use Cwd ;
use Getopt::Long ;
use lib ( File::Basename::dirname($0) ) ;
use cmake ;

my $prefix = File::Basename::basename($0) ;

my %opt = () ;
if( !GetOptions( \%opt , "help|h" , "makefile=s" , "config=s" , "arch=s" , "cmake:s" , "quiet|q" ) ||
	$opt{help} )
{
	print "usage: $prefix [--config={debug|release}] [--arch={x64|x86}] [<source-dir> [<build-dir>]]\n" ;
	exit( $opt{help} ? 0 : 1 ) ;
}

my $arch = $opt{arch} || $ENV{Platform} || "" ;
my $os_arch = $arch || lc($^O) ;
my $config = $opt{config} || "release" ;
my $src_dir = $ARGV[0] || "libressl" ;
my $build_dir = $ARGV[1] || "libressl-${os_arch}" ;
my $quiet = $opt{quiet} ;
my $makefile = $opt{makefile} || (dirname($0)."/libresslbuild.mak") ;
( my $cmake = $opt{cmake} || cmake::pick() ) =~ s/"//g ;

_warn( "unknown architecture [$arch]" ) if( $arch && $arch ne "x64" && $arch ne "x86" ) ;
_die( "invalid build type [$config]" ) if( $config ne "debug" && $config ne "release" ) ;

_log( "source-dir=$src_dir" ) ;
_log( "build-dir=$build_dir" ) ;
_log( "makefile=$makefile" ) ;
_log( "cmake=$cmake" ) ;

-f "$src_dir/ssl/ssl_init.c" or _die( "invalid libressl source directory [$src_dir]" ) ;

my $build_dir_parent = File::Basename::dirname( $build_dir ) ;
_mkdir( $build_dir_parent ) if( ! -d $build_dir_parent ) ;
_mkdir( $build_dir ) ;

# "cmake -B -S"
{
	my @cmake_options = () ;

	my $a = $arch eq "x86" ? "Win32" : $arch ;
	push @cmake_options , ("-A",$a) if $a ;
	push @cmake_options , "-DCMAKE_MAKE_PROGRAM=/usr/bin/make" if( $^O eq "linux" ) ; # not gmake
	push @cmake_options , "-DCMAKE_BUILD_TYPE=$config" ;
	push @cmake_options , "-DLIBRESSL_APPS=Off" ;
	push @cmake_options , "-DUSE_STATIC_MSVC_RUNTIMES=On" ;

	_log( "$cmake ".join(" ",@cmake_options)." -B $build_dir -S $src_dir" ) ;
	system( $cmake , @cmake_options ,
		"-B" , $build_dir ,
		"-S" , $src_dir ) == 0
			or _die( "cmake failed" ) ;
}

# "cmake --build"
{
	_log( "$cmake --build $build_dir --config $config" ) ;
	system( $cmake , "--build" , $build_dir , "--config" , $config ) == 0
		or _die( "cmake build failed" ) ;
}

_copy_libraries( $build_dir ) ;

_log( "done [$build_dir]" ) ;
exit( 0 ) ;

sub _mkdir
{
	my ( $dir ) = @_ ;
	mkdir( $dir ) ;
	-d $dir or die "$prefix: error: cannot create directory [$dir]: $!\n" ;
}

sub _copy_libraries
{
	my ( $build_dir ) = @_ ;
	my @libs = (
		"$build_dir/crypto/"._libname("crypto") ,
		"$build_dir/ssl/"._libname("ssl") ,
	) ;
	_mkdir( "$build_dir/library" ) ;
	_mkdir( "$build_dir/library/$config" ) ;
	for my $lib ( @libs )
	{
		File::Copy::copy( $lib , "$build_dir/library/$config/" )
			or _die( "library copy failed [$lib]: $!" ) ;
	}
}

sub _libname
{
	my ( $name ) = @_ ;
	if( $^O =~ m/win/i )
	{
		return "$config/${name}.lib" ;
	}
	else
	{
		return "lib${name}.a" ;
	}
}

sub _die
{
	my ( $s ) = @_ ;
	die "$prefix: error: $s\n" ;
}

sub _warn
{
	my ( $s ) = @_ ;
	warn "$prefix: warning: $s\n" ;
}

sub _log
{
	print "$prefix: " , @_ , "\n" unless $quiet ;
}

