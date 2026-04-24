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
# mbedtlsbuild.pl
#
# Builds MbedTLS using cmake with "/MT" used on Windows for a
# statically-linked run-time library.
#
# usage: mbedtlsbuild.pl [<options>] [<src-dir> [<build-dir>]]
#         --arch={x64|x86}            build architecture
#         --config={release|debug}    build type
#         --cmake=<path>              cmake
#         --tls13                     enable TLS 1.3 (for MbedTLS 2.x)
#         <src-dir>                   source directory
#         <build-dir>                 build directory
#
# The command-line directories default as follows:
#     src-dir      - ancestor($0)/mbedtls (searches upwards)
#     build-dir    - <cwd>/mbedtls-<arch>
#
# All mbedtls include files are copied into the build tree so that the
# mbedtls configuration header (mbedtls_config.h) can be edited to
# enable TLS 1.3 and so that client code can build with reference to
# a single base directory.
#
# Libraries and headers end up in:
#     libs      - <build-dir>/library/<config>
#     headers   - <build-dir>/include/{mbedtls,psa}
#
# Also usable as a perl module, as follows.
#
# Synopsis:
#   require "mbedtlsbuild.pl" ;
#   MbedtlsBuild::copy_headers( $src_dir , $build_dir ) ;
#   my $config_file = MbedtlsBuild::config_file($build_dir) ;
#   MbedtlsBuild::configure( $config_file , $tls13 ) ; # (optional)
#   MbedtlsBuild::new(...)->build() ; # or...
#   my @cmake_options = MbedtlsBuild::cmake_options( $src_dir , $config_file , $arch ) ;
#   cmake @cmake_options -B ... -S ...
#

use strict ;
use IO::File ;
use Getopt::Long ;
use File::Basename ;
use File::Path ;
use File::Copy ;
use Cwd ;
use lib ( File::Basename::dirname($0) ) ;
use cmake ;

package MbedtlsBuild ;

sub new
{
	my ( $classname , $src_dir , $build_dir , $arch , $config , $tls13 , $prefix , $quiet , $os , $cflags_extra ) = @_ ;

	$arch ||= "" ;
	$config ||= "release" ;
	$prefix ||= "mbedtlsbuild" ;
	$cflags_extra ||= "" ;
	$os ||= os() ;

	my $this = bless {
		m_prefix => $prefix ,
		m_quiet => $quiet ,
		m_arch => $arch ,
		m_config => $config ,
		m_tls13 => $tls13 ,
		m_src_dir => $src_dir ,
		m_build_dir => $build_dir ,
		m_os => $os ,
		m_dot_obj => ( $os eq "windows" ? ".obj" : ".o" ) ,
		m_dot_lib => ( $os eq "windows" ? ".lib" : ".a" ) ,
		m_lib_prefix => ( $os eq "windows" ? "" : "lib" ) ,
		m_cflags_extra => $cflags_extra ,
	} , $classname ;
	_init( $this ) ;
	return $this ;
}

sub os
{
	my $os = ( $^O =~ m/win/i ? "windows" : "unix" ) ;
	return $os ;
}

sub _init
{
	my ( $this ) = @_ ;
	-f "$$this{m_src_dir}/library/aes.c" or die "$$this{m_prefix}: error: invalid mbedtls source directory [$$this{m_src_dir}]\n" ;
	map { -d $_ || mkdir $_ or die "mkdir($_)" } (
		File::Basename::dirname($this->{m_build_dir}) ,
		"$$this{m_build_dir}" ,
		"$$this{m_build_dir}/library" ,
		"$$this{m_build_dir}/library/$$this{m_config}" ,
	) ;
}

sub find
{
	my ( $name ) = @_ ;
	my $base = File::Basename::dirname( $0 ) ;
	for my $path ( "$base/$name" , "$base/../$name" , "$base/../../$name" )
	{
		return $path if -d $path && -e "$path/include/mbedtls/ssl.h" && -e "$path/library/version.c" ;
	}
	return $name ;
}

sub _read_objects
{
	my ( $this , $key ) = @_ ;
	my $fh = new IO::File( "$$this{m_src_dir}/library/Makefile" ) or die ;
	my $x ; { local $/ = undef ; $x = <$fh> ; }
	my ( $obj ) = ( $x =~ m/OBJS_${key}\s*=\s*([^#]*)/m ) ;
	return $obj
		=~ s/[\n\t\\]/ /rg
		=~ s/\s+/ /rg ;
}

sub _objects
{
	my ( $this , $lib ) = @_ ;
	return split( " " , $this->_read_objects(uc($lib)) ) ;
}

sub _sources
{
	my ( $this , $lib ) = @_ ;
	return map { $_ =~ s/\.o/.c/g ; $_ } split(" ",$this->_read_objects(uc($lib))) ;
}

sub _have_msvc_static_runtime_option
{
	my ( $src_dir ) = @_ ;
	my $fh = new IO::File( "$src_dir/library/CMakeLists.txt" ) or die ;
	my $x = eval { local $/ ; <$fh> } ;
	my $result = ( $x =~ m/MSVC_STATIC_RUNTIME/ ) ; # not in 2.28.x
	return $result ;
}

sub _cmake_static_build_options
{
	my ( $src_dir , $os ) = @_ ;
	my @options = () ;
	if( $os ne "unix" )
	{
		if( _have_msvc_static_runtime_option($src_dir) )
		{
			push @options , "-DMSVC_STATIC_RUNTIME=On" ;
		}
		else
		{
			push @options , "-DCMAKE_C_FLAGS_DEBUG=\"-MTd -Ob0 -Od -RTC1\"" ; # must use dashes here
			push @options , "-DCMAKE_C_FLAGS_RELEASE=\"-MT -O2 -Ob1 -DNDEBUG\"" ;
		}
	}
	return @options ;
}

sub cmake_options
{
	my ( $src_dir , $config_file , $arch , $os , $build_type ) = @_ ;
	$os ||= os() ;
	die "undefined architecture" if( !$arch && ( $os eq "windows" ) ) ;
	my @options = () ;
	my $a = $arch eq "x86" ? "Win32" : $arch ;
	push @options , ("-A",$a) if $a ;
	push @options , _cmake_static_build_options( $src_dir , $os ) ;
	push @options , "-DCMAKE_BUILD_TYPE=$build_type" if $build_type ;
	push @options , "-DENABLE_TESTING=Off" ;
	push @options , "-DENABLE_PROGRAMS=Off" ;
	push @options , "-DMBEDTLS_FATAL_WARNINGS=Off" ; # for eg. v3.5.1 with TLS1.3 enabled
	push @options , "-DMBEDTLS_CONFIG_FILE=$config_file" if $config_file ;
	push @options , "-DCMAKE_MAKE_PROGRAM=/usr/bin/make" if( $os eq "unix" ) ;
	return @options ;
}

sub copy_headers_
{
	my ( $this ) = @_ ;
	$this->_log( "copy $$this{m_src_dir}/include/{mbedtls,psa}/*.h " .
		"$$this{m_build_dir}/include/{mbedtls,psa}/" ) ;
	copy_headers( $this->{m_src_dir} , $this->{m_build_dir} ) ;
}

sub copy_headers
{
	# see also winbuild.pl
	my ( $src_dir , $build_dir ) = @_ ;
	my @subdirs = ( "mbedtls" , "psa" ) ;
	map { _copy_headers_imp( "$src_dir/include" , "$build_dir/include" , $_ ) } @subdirs ;
}

sub _copy_headers_imp
{
	my ( $src_inc_dir , $dst_inc_dir , $subdir ) = @_ ;

	mkdir( $dst_inc_dir ) || die if ! -d $dst_inc_dir ;
	mkdir( "$dst_inc_dir/$subdir" ) || die if ! -d "$dst_inc_dir/$subdir" ;

	for my $header ( glob("$src_inc_dir/$subdir/*.h") )
	{
		if( -f $header )
		{
			File::Copy::copy( $header , "$dst_inc_dir/$subdir/" ) or die ;
		}
	}
}

sub config_file
{
	my ( $base_dir ) = @_ ;
	my $path = "$base_dir/include/mbedtls/mbedtls_config.h" ; # not in mbedtls v2.x
	$path = undef if ! -e $path ;
	return $path ;
}

sub configure_
{
	my ( $this ) = @_ ;
	my $config_file = config_file( $this->{m_build_dir} ) ;
	configure( $config_file , $this->{m_tls13} ) ;
}

sub configure
{
	my ( $config_file , $tls13 ) = @_ ;

	my $config_file_in = $config_file ;
	my $config_file_out = $config_file ;
	( $config_file_in , $config_file_out ) = @$config_file if ref($config_file) ;

	return if !$config_file_in ;

	my $fh = new IO::File( $config_file_in ) or die ;
	my $config_in = eval { local $/ ; <$fh> } ;
	$fh->close() or die ;

	my $message = "added by $0:" ;
	my $config_out = $config_in ;

	if( $tls13 )
	{
		$config_out =~ s;^//\s*(#define\s+MBEDTLS_SSL_PROTO_TLS1_3) *([\r]?)$;//$1$2\n// $message$2\n$1$2\n;m ;
		$config_out =~ s;^//\s*(#define\s+MBEDTLS_SSL_TLS1_3_COMPATIBILITY_MODE) *([\r]?)$;//$1$2\n// $message$2\n$1$2\n;m ;
	}

	if( $config_file_in eq $config_file_out && $config_in eq $config_out )
	{
		# no-op
	}
	else
	{
		$fh = new IO::File( $config_file_out , "w" ) or die ;
		print $fh $config_out , "\n" ;
		$fh->close() or die ;
	}
}

sub build
{
	my ( $this , $cmake ) = @_ ;
	$cmake ||= "cmake" ;

	# "cmake -B -S"
	{
		$ENV{CFLAGS} = $this->{m_cflags_extra} if $this->{m_cflags_extra} ;

		my @cmake_options = cmake_options( $this->{m_src_dir} ,
			config_file($this->{m_build_dir}) , $this->{m_arch} ,
			$this->{m_os} , $this->{m_config} ) ;

		push @cmake_options , ( "-B" , $this->{m_build_dir} ) ;
		push @cmake_options , ( "-S" , $this->{m_src_dir} ) ;

		$this->_log( "set CFLAGS=$$this{m_cflags_extra}" ) if $this->{m_cflags_extra} ;
		$this->_log( "$cmake " . join(" ",@cmake_options) ) ;

		system( $cmake , @cmake_options ) == 0
			or die "$$this{m_prefix}: error: cmake failed\n" ;
	}

	# "cmake --build"
	{
		my @cmake_options = () ;
		push @cmake_options , ( "--build" , $this->{m_build_dir} ) ;
		push @cmake_options , ( "--target" , "lib" ) ; # or mbedtls, mbedcrypto, mbedx509
		push @cmake_options , ( "--config" , $this->{m_config} ) ;

		$this->_log( "$cmake " . join(" ",@cmake_options) ) ;

		system( $cmake , @cmake_options ) == 0
			or die "$$this{m_prefix}: error: cmake build failed\n" ;
	}
}

sub clean
{
	my ( $build_dir ) = @_ ;
	unlink "$build_dir/CMakeCache.txt" ;
	File::Path::remove_tree( "$build_dir/CMakeFiles" , {safe=>1,verbose=>0} ) ;
}

sub _log
{
	my ( $this , @args ) = @_ ;
	print "$$this{m_prefix}: " , @args , "\n" unless $this->{m_quiet} ;
}

1 ;

# ==

package main ;
use Getopt::Long ;

if( basename($0) eq "mbedtlsbuild.pl" )
{
	my $prefix = File::Basename::basename($0) ;

	my %opt = () ;
	if( !GetOptions( \%opt , "help|h" , "config=s" , "arch=s" , "cmake:s" , "tls13" , "quiet|q" , "cflags-extra=s" , "as-windows" ) ||
		$opt{help} )
	{
		print "usage: $prefix [--config={debug|release}] [--arch={x64|x86}] [<source-dir> [<build-dir>]]\n" ;
		exit( $opt{help} ? 0 : 1 ) ;
	}

	my $arch = $opt{arch} || $ENV{Platform} || "" ;
	my $os_arch = $arch || lc($^O) ;
	my $config = $opt{config} || "release" ;
	my $tls13 = $opt{tls13} ;
	my $src_dir = $ARGV[0] || MbedtlsBuild::find("mbedtls") ;
	my $build_dir = $ARGV[1] || "mbedtls-${os_arch}" ;
	my $quiet = $opt{quiet} ;
	my $os = ( $opt{'as-windows'} ? "windows" : undef ) ;
	my $cflags_extra = $opt{'cflags-extra'} ;
	( my $cmake = $opt{cmake} || cmake::pick() ) =~ s/"//g ;

	warn "$prefix: error: unknown architecture [$arch]\n" if( $arch && $arch ne "x64" && $arch ne "x86" ) ;
	die "$prefix: error: invalid build type [$config]\n" if( $config ne "debug" && $config ne "release" ) ;

	print "$prefix: source-dir=$src_dir\n" unless $quiet ;
	print "$prefix: build-dir=$build_dir\n" unless $quiet ;
	print "$prefix: cmake=$cmake\n" unless $quiet ;

	my $b = new MbedtlsBuild( $src_dir , $build_dir , $arch , $config , $tls13 , $prefix , $quiet , $os , $cflags_extra ) ;
	$b->copy_headers_() ;
	$b->configure_() ;
	my $ok = $b->build( $cmake ) ;
	if( $ok )
	{
		print "$prefix: done [$build_dir]\n" unless $quiet ;
		exit( 0 ) ;
	}
	else
	{
		print "$prefix: error: failed\n" unless $quiet ;
		exit( 1 ) ;
	}
}
else
{
	1 ;
}

