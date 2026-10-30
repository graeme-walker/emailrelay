#!/usr/bin/env perl
#
# SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
# SPDX-License-Identifier: GPL-3.0-or-later
#
# qtbuild.pl
#
# Builds Qt libraries in the "qtbase" submodule, typically on Windows.
# Also builds tools from the "qttools" submodule and translations
# from "qttranslations". The build is for statically-linked binaries
# by default (see "--static" and "--dynamic").
#
# Works for either Qt5 or Qt6, but note that Qt6 requires c++17 and
# cmake.
#
# Only the "qtbase" submodule is required to build the Qt Widgets
# libraries; the "qttools" submodule provides "windeployqt.exe"
# on Windows, which might be needed after a non-static build, and
# also "lrelease.exe", which is needed to build the translations;
# and the "qttranslations" submodule provides the translation
# ".qm" files, making use of "lrelease.exe".
#
# Static libraries are required so that GUI applications can be
# distributed without any additional DLLs or compiler run-time files.
# Qt is available as a binary download for Windows, but only with
# dynamic libraries.
#
# usage: qtbuild.pl [<options>] [<src-dir> [<build-dir>] [<install-dir>]]]
#         --arch={x64|x86}            build architecture (windows)
#         --config={debug|release}    build type
#         --cmake=<path>              cmake
#         --dynamic                   dynamic linking
#         --static                    static linking (default, for symmetry)
#
# The command-line directories default as follows:
#     src-dir      - qt-src/
#     build-dir    - qt-build-<arch>-<config>[-dynamic]/
#     install-dir  - qt-bin-<arch>[-dynamic]/
#
# Libraries, headers and tools end up in:
#     libs      - <install-dir>/lib
#     headers   - <install-dir>/include
#     tools     - <install-dir>/bin
#
# Runs the "qtbase" configure script with options chosen to limit
# the scope of the build, and then "make" and "make install",
# followed by "cmake", "cmake --build" and "cmake -- install"
# in the other two submodules.
#
# Download Qt6 tarballs, eg:
#    $ QT=https://download.qt.io/official_releases/qt/6.9/6.9.1
#    $ wget $QT/submodules/qtbase-everywhere-src-6.9.1.tar.xz
#    $ wget $QT/submodules/qttools-everywhere-src-6.9.1.tar.xz
#    $ wget $QT/submodules/qttranslations-everywhere-src-6.9.1.tar.xz
#    $ etc.
#
# There will be no top-level configure script if downloading
# submodule tarballs separately so this script always runs the
# configure script in the "qtbase" submodule directly and then
# runs "cmake" in the other two.
#
# Delete the build directory tree for a clean build ("rmdir /q /s"
# on Windows).
#

use strict ;
use Cwd ;
use File::Basename ;
use File::Copy ;
use IO::File ;
use Getopt::Long ;
use lib ( File::Basename::dirname($0) ) ;
use cmake ;

my $cfg_prefix = File::Basename::basename($0) ;
my %opt = () ;
if( !GetOptions( \%opt ,
	"help|h" , "cmake:s", "arch=s", "config=s", "verbose|v", "quiet|q", "dynamic" , "static" ) ||
	$opt{help} )
{
	print "usage: $cfg_prefix [--config={debug|release}] [--arch={x86|x64}] [<src-dir> [<build-dir>] [<install-dir>]]]\n" ;
	exit( $opt{help} ? 0 : 2 ) ;
}
die if ( $opt{dynamic} && $opt{static} ) ;
my $_linkage = ( $opt{dynamic} ? "-dynamic" : "" ) ;
my $cfg_static = !$opt{dynamic} ;
my $cfg_config = $opt{config} || "release" ;
my $cfg_arch = $opt{arch} || $ENV{Platform} || "" ;
my $cfg_os_arch = $cfg_arch || lc($^O) ;
my $cfg_source_dir_ = $ARGV[0] || "qt-src" ;
my $cfg_build_dir = $ARGV[1] || "qt-build-${cfg_os_arch}-${cfg_config}${_linkage}" ;
my $cfg_install_dir = $ARGV[2] || "qt-bin-${cfg_os_arch}${_linkage}" ;
my $cfg_source_dir = Cwd::realpath( $cfg_source_dir_ ) ;
my $cfg_verbose = $opt{verbose} ; # (verbose make)
my $cfg_quiet = $opt{quiet} ; # (this script)
my $cfg_generator = "Ninja" ; # this is the only supported generator
( my $cfg_cmake = $opt{cmake} || cmake::pick() ) =~ s/"//g ;
die "$cfg_prefix: invalid build type [$cfg_config]\n" if( $cfg_config ne "debug" && $cfg_config ne "release" ) ;

my @submodules = ( "qtbase" , "qttools" , "qttranslations" ) ;

# sanity checks
die "$cfg_prefix: error: no source directory [$cfg_source_dir_]\n" if ! -d $cfg_source_dir ;
die "$cfg_prefix: error: undefined architecture\n" if( _windows() && !$cfg_arch ) ;
my @check_files = (
	"$cfg_source_dir/qtbase/configure" ,
	"$cfg_source_dir/qtbase/configure.bat" ,
	"$cfg_source_dir/qtbase/src/corelib/kernel/qobject.h" ,
) ;
my $cfg_qt5 = ( -e "$cfg_source_dir/qtbase/qtbase.pro" ) ;
if( !$cfg_qt5 )
{
	push @check_files , "$cfg_source_dir/qttools/CMakeLists.txt" ;
	push @check_files , "$cfg_source_dir/qttranslations/CMakeLists.txt" ,
}
for my $check ( @check_files )
{
	die "$cfg_prefix: error: missing source file [$check]\n" if ! -e $check ;
}

# preferably start with a clean build directory
if( -d $cfg_build_dir && scalar(glob("$cfg_build_dir/*")) )
{
	warn "$cfg_prefix: warning: build directory is not empty [$cfg_build_dir]\n" ;
}

# create base directories
_mkdir( $cfg_build_dir ) or die "$cfg_prefix: error: cannot create build directory [$cfg_build_dir]\n" ;
_mkdir( $cfg_install_dir ) or die "$cfg_prefix: error: cannot create install directory [$cfg_install_dir]\n" ;
$cfg_build_dir = Cwd::realpath( $cfg_build_dir ) ;
$cfg_install_dir = Cwd::realpath( $cfg_install_dir ) ;

print "$cfg_prefix: source-dir=$cfg_source_dir\n" unless $cfg_quiet ;
print "$cfg_prefix: build-dir=$cfg_build_dir\n" unless $cfg_quiet ;
print "$cfg_prefix: install-dir=$cfg_install_dir\n" unless $cfg_quiet ;
print "$cfg_prefix: cmake=$cfg_cmake\n" unless $cfg_quiet ;

if( $cfg_qt5 )
{
	_touch( "$cfg_source_dir/qtbase/.git" ) ;
	_touch( "$cfg_source_dir/qttools/.git" ) ;
	_touch( "$cfg_source_dir/qttranslations/.git" ) ;
}

# prepare the 'configure' options -- see qtbase/config_help.txt
my @configure_args = grep {m/./} (
		"-opensource" , "-confirm-license" ,
		"-prefix" , $cfg_install_dir ,
		"-${cfg_config}" ,
		( $cfg_static ? "-static" : "" ) ,
		( $cfg_static && _windows() ? "-static-runtime" : "" ) ,
		"-platform" , ( _windows() ? "win32-msvc" : "linux-g++" ) ,
		"-make" , "libs" ,
		"-make" , "tools" ,
		"-nomake" , "examples" ,
		"-nomake" , "tests" ,
		"-no-opengl" ,
		"-no-openssl" ,
		"-no-dbus" ,
		"-no-gif" ,
		"-no-libpng" ,
		"-no-libjpeg" ,
		( $cfg_qt5 ? "" : "-cmake-generator" ) ,
		( $cfg_qt5 ? "" : $cfg_generator ) ,
) ;
if( _unix() )
{
	push @configure_args , (
		"-xcb" ,
		"-feature-thread" ,
		"-feature-xcb" ,
		"-feature-xkbcommon-x11" ,
	) ;
}
if( $cfg_verbose )
{
	unshift @configure_args , "-verbose" ;
}
if( $cfg_config ne "debug" )
{
	push @configure_args , (
		"-no-pch" ,
		"-optimize-size"
	) ;
}

# make sure the requested cmake is first on the path since the
# configure scripts just runs plain "cmake"
if( File::Basename::dirname($cfg_cmake) ne "." )
{
	my $dir = Cwd::realpath( File::Basename::dirname($cfg_cmake) ) ;
	my $sep = _windows() ? ";" : ":" ;
	my @path = split( $sep , $ENV{PATH} ) ;
	$ENV{PATH} = join( $sep , $dir , @path ) ;
}

# run "configure" at the "qtbase" submodule level
#
{
	my $base_build_dir = "$cfg_build_dir/qtbase" ;
	_mkdir( $base_build_dir ) or die "$cfg_prefix: error: cannot create the qtbase build directory [$base_build_dir]" ;
	$ENV{MAKE} = "make" if( _unix() ) ; # we dont want gmake -- see "qtbase/configure"
	$ENV{PATH} = File::Basename::dirname($^X).";$ENV{PATH}" if( _windows() ) ; # ensure perl is on the path
	my $configure_script = _nativepath( "$cfg_source_dir/qtbase/configure".(_unix()?"":".bat") ) ;
	run( $base_build_dir , "configure($cfg_os_arch)" , $configure_script , @configure_args ) ;
}

# "cmake", "cmake --build", "cmake --install" for each submodule
#
for my $submodule ( @submodules )
{
	my $sub_build_dir = "$cfg_build_dir/$submodule" ;
	if( $submodule ne "qtbase" ) # if qttools or qttranslations...
	{
		# submodule configuration
		_mkdir( $sub_build_dir ) or die "$cfg_prefix: error: cannot create $submodule build directory [$sub_build_dir]\n" ;
		run( $sub_build_dir , "configure($cfg_os_arch,$submodule)" , _configure_command($submodule) ) ;
	}

	# submodule build
	run( $sub_build_dir , "make($cfg_os_arch,$submodule)" , _build_command() ) ;

	# submodule install
	run( $sub_build_dir , "make-install($cfg_os_arch,$submodule)" , _install_command() ) ;
}
print "$cfg_prefix: done [$cfg_install_dir]\n" unless $cfg_quiet ;

## ==

sub _configure_command
{
	my ( $submodule ) = @_ ;
	return (
		_nativepath( "$cfg_build_dir/qtbase/qmake/qmake".(_unix()?"":".exe") ) ,
		"-qtconf" , "$cfg_build_dir/qtbase/bin/qt.conf" ,
		"$cfg_source_dir/$submodule/$submodule.pro" ) if $cfg_qt5 ;
	return (
		_nativepath( "$cfg_install_dir/bin/qt-configure-module".(_unix()?"":".bat") ) ,
		"$cfg_source_dir/$submodule" ) ; # possibly add "-feature-foo" or "-no-feature-bar" here
}

sub _build_command
{
	return ( "nmake" ) if( $cfg_qt5 && _windows() ) ;
	return ( "make" ) if $cfg_qt5 ;
	return ( $cfg_cmake , "--build" , "." , "--config" , $cfg_config , "--parallel" ) ;
}

sub _install_command
{
	return ( "nmake" , "install" ) if( $cfg_qt5 && _windows() ) ;
	return ( "make" , "install" ) if $cfg_qt5 ;
	return (
		$cfg_cmake , "--install" , "." , "--config" ,  $cfg_config ,
		"--prefix" , $cfg_install_dir ) ; # (buggy qt cmake file requires --prefix)
}

sub run
{
	my ( $cd , $log_prefix , @cmd ) = @_ ;

	my $old_dir ;
	if( $cd )
	{
		$old_dir = Cwd::getcwd() ;
		chdir( $cd ) or die "$cfg_prefix: error: cannot cd to [$cd]" ;
	}
	_run_imp( $log_prefix , \@cmd ) ;
	if( $old_dir )
	{
		chdir( $old_dir ) or die "$cfg_prefix: error: cannot cd back to [$old_dir]" ;
	}
}

sub _run_imp
{
	my ( $log_prefix , $cmd ) = @_ ;

	print "$log_prefix: running: cmd=[".join(" ",@$cmd)."] cwd=[".Cwd::getcwd()."]\n" ;
	my $rc = system( @$cmd ) ;
	print "$log_prefix: rc=[$rc]\n" ;
	die "$cfg_prefix: error: command failed\n" if $rc != 0 ;
}

sub _touch
{
	my ( $path ) = @_ ;
	my $fh = new IO::File( $path , "w" ) ;
	$fh && $fh->close() ;
}

sub _mkdir
{
	my ( $dir ) = @_ ;
	return 1 if ( -d $dir ) ;
	return mkdir( $dir ) ;
}

sub _unix
{
	return !_windows() ;
}

sub _windows
{
	my $w = ( $^O =~ m/win/i ) ;
	return $w ;
}

sub _nativepath
{
	my ( $path ) = @_ ;
	$path =~ s;/;\\;g if _windows() ;
	return $path ;
}

