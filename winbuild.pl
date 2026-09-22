#!/usr/bin/env perl
#
# SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
# SPDX-License-Identifier: GPL-3.0-or-later
#
# winbuild.pl
#
# Builds emailrelay and mbedtls on Windows from source using cmake.
# Looks for pre-built Qt libraries and builds the emailrelay GUI if Qt
# libraries are available. Also looks for pre-built openssl libraries
# (which should have static run-time linkage) and builds emailrelay
# with openssl support if they are available. Does a x64 release build
# by default.
#
# usage:
#          winbuild.pl [<subtask> [<subtask> ...]]
#          winbuild.pl --all [options] <src-dir> <build-dir>
#
# The "--all" usage is to support "winbuildall.bat", using environment
# variables to bypass the library searching.
#
# Parses automake files throughout the emailrelay source tree to generate
# windows-specific cmake files, then uses cmake to generate mbedtls and
# emailrelay makefiles, and finally "cmake --build" to build the mbedtls
# libraries and emailrelay executables.
#
# Use "windbuild.bat configure" to create a template configuration file, and
# "winbuild.bat download-mbedtls" to download mbedtls source.
#
# Spits out additional batch files for doing sub-tasks, including:
#    winbuild-generate.bat - generates CMakeLists.txt files for cmake
#    winbuild-cmake.bat    - uses cmake to generate makefiles
#    winbuild-build.bat    - builds mbedtls and emailrelay
#    winbuild-test.bat     - runs tests
#    winbuild-vclean.bat   - cleans up
#    winbuild-install.bat  - runs winbuild-assembly.pl
#
# The generated emailrelay cmake files specify static linkage of the run-time
# library ("/MT"), with the exception of the emailrelay GUI which is built
# with "/MD" by default.
#
# Requires "cmake" to be on the path or somewhere obvious (see find_cmake()
# in "winbuild.pm").
#
# Looks for mbedtls source code in a sibling or child directory (see
# winbuild::find_mbedtls_src()) and builds it using cmake (and "/MT") in an
# out-of-tree build directory "<this-dir>/mbedtls-<arch>". The mbedtls
# headers, including the configuration header, are copied into the build
# tree and the configuration header is edited as necessary.
#
# Download mbedtls source from GitHub, eg:
#     curl -L -O https://github.com/Mbed-TLS/mbedtls/archive/refs/tags/v3.6.3.1.tar.gz
#     tar -xf v3.6.3.1.tar.gz
#
# Looks for Qt libraries in various places (see winbuild::find_qt_x64()).
# By default the emailrelay GUI builds with dynamic linkage to Qt DLLs.
# For a statically linked emailrelay GUI the Qt libraries will normally
# have to be built from source (see qtbuild.pl).
#
# When compiling the basic "-I" and "-D" options come from parsing the
# automake makefiles ("Makefile.am"). Additional compiler flags are
# injected by the perl module "BuildInfo.pm". The results are
# written into the cmake target_...() directives in the various
# "CMakeLists.txt" files. Then cmake adds its own options which are
# stored in the "CMakeCache.txt" file and overridden from the cmake
# command-line with "-DCMAKE_CXX_FLAGS=..." etc. (see run_cmake()
# below).
#

use strict ;
use Cwd ;
use IO::File ;
use File::Find ;
use File::Basename ;
use File::Copy ;
use File::Glob ;
use File::Path ;
use lib ( File::Basename::dirname($0) , File::Basename::dirname($0)."/libexec" ) ;
use BuildInfo ;
use cmake ;
require "make2cmake" ;
require "winbuild.pm" ;
require "mbedtlsbuild.pl" ;

if( $ARGV[0] eq "--all" )
{
	winbuildall() ;
	exit( 0 ) ;
}

# configuration -- edit here as required or via winbuild.cfg
my %cfg = (
	verbose => 1 , # show compiler commands
	debug => 0 , # do debug builds
	release => 1 , # do release builds
	with_mbedtls => 1 , # build and use mbedtls
	with_openssl => 0 , # use openssl libraries, pre-built with /MT
	with_gui => 0 , # build emailrelay GUI, linked with qt libraries
	x64 => 1 , # build for x64 architecture
	x86 => 0 , # build for 32-bit x86 architecture
	cmake => undef , # cmake path
	mbedtls_src => undef , # mbedtls source tree
	openssl_x64 => undef , # openssl binary tree for x64
	openssl_x86 => undef , # openssl binary tree for x86
	qt_x64 => undef , # qt binary tree for x64
	qt_x86 => undef , # qt binary tree for x86
	qt_static => undef , # build emailrelay GUI with /MT and link with static qt libraries
) ;
sub _create_winbuild_cfg
{
	my $fh = new IO::File( "winbuild.cfg" , "w" ) or die ;
	print $fh "# winbuild.cfg -- created by winbuild.pl\n" ;
	print $fh "verbose=1\n" ;
	print $fh "debug=0\n" ;
	print $fh "release=1\n" ;
	print $fh "with_mbedtls=1\n" ;
	print $fh "with_openssl=0\n" ;
	print $fh "with_gui=0\n" ;
	print $fh "x64=1\n" ;
	print $fh "x86=0\n" ;
	print $fh "#cmake=...\n" ;
	print $fh "#mbedtls_src=...\n" ;
	print $fh "#openssl_x64=...\n" ;
	print $fh "#openssl_x86=...\n" ;
	print $fh "#qt_x64=...\n" ;
	print $fh "#qt_x86=...\n" ;
	print $fh "#qt_static=0\n" ;
	$fh->close() or die ;
}
if( $ARGV[0] eq "configure" )
{
	_create_winbuild_cfg() ;
	print "winbuild: winbuild.cfg created: edit as required\n" ;
	winbuild::create_touchfile( winbuild::default_touchfile($0) ) ;
	exit( 0 ) ;
}
elsif( $ARGV[0] eq "download-mbedtls" )
{
	my $filename = "v3.6.3.1.tar.gz" ;
	my $rc1 = system( "curl -L -O https://github.com/Mbed-TLS/mbedtls/archive/refs/tags/$filename" ) ;
	my $rc2 = system( "tar -xf $filename" ) ;
	die "error: mbedtls download failed" if( $rc1 != 0 || $rc2 != 0 ) ;
	winbuild::create_touchfile( winbuild::default_touchfile($0) ) ;
	exit( 0 ) ;
}
{
	my $fh = new IO::File( "winbuild.cfg" , "r" ) ;
	while(<$fh>)
	{
		chomp( my $line = $_ ) ;
		$line =~ s/#.*// ;
		$line =~ s/\s*$// ;
		my ( $key , $value ) = ( $line =~ m/(\S+)[\s=]+(.*)/ ) ;
		$key =~ s/-/_/g ;
		$value =~ s/\s*$// ;
		$value =~ s/^"// ;
		$value =~ s/"$// ;
		$cfg{$key} = $value ;
	}
}
die unless ($cfg{x64} || $cfg{x86}) ;

my @cfg_run_parts = @ARGV ;
if( scalar(@cfg_run_parts) == 0 )
{
	@cfg_run_parts = ( $cfg{with_mbedtls} ?
		qw( batchfiles generate mbedtls cmake build ) :
		qw( batchfiles generate cmake build ) ) ;
}

# find stuff ...
if( !$cfg{cmake} ) { $cfg{cmake} = winbuild::find_cmake() }
if( $cfg{with_mbedtls} && !$cfg{mbedtls_src} ) { $cfg{mbedtls_src} = winbuild::find_mbedtls_src() }
if( $cfg{with_gui} && !$cfg{qt_x64} && $cfg{x64} ) { $cfg{qt_x64} = winbuild::find_qt_x64() }
if( $cfg{with_gui} && !$cfg{qt_x86} && $cfg{x86} ) { $cfg{qt_x86} = winbuild::find_qt_x86() }

my $missing_cmake = ( !$cfg{cmake} || !-e $cfg{cmake} ) ;
my $missing_qt =
	( $cfg{with_gui} && $cfg{x86} && ( !$cfg{qt_x86} || !-d "$cfg{qt_x86}/lib" ) ) ||
	( $cfg{with_gui} && $cfg{x64} && ( !$cfg{qt_x64} || !-d "$cfg{qt_x64}/lib" ) ) ;
my $missing_mbedtls = ( $cfg{with_mbedtls} && ( !$cfg{mbedtls_src} || !-d "$cfg{mbedtls_src}/include" ) ) ;
if( $missing_cmake )
{
	warn "error: cannot find cmake.exe: " .
		"add to PATH or download from cmake.org\n" ;
}
if( $missing_qt )
{
	warn "error: cannot find qt libraries: " .
		"download from wwww.qt.io " .
		"or set with_gui=0 in winbuild.cfg\n" ;
}
if( $missing_mbedtls )
{
	warn "error: cannot find mbedtls source: " .
		"download with \"winbuild.bat download-mbedtls\" " .
		"or set with_mbedtls=0 in winbuild.cfg\n"
}
if( $missing_qt || $missing_mbedtls )
{
	warn "winbuild: info: run \"winbuild.bat configure\" to create winbuild.cfg\n" if ! -e "winbuild.cfg" ;
	warn "winbuild: error: missing prerequisites: please install the missing components " ,
		"or edit winbuild.cfg" , "\n" ;
	die "winbuild: error: missing prerequisites\n" ;
}
if( $cfg{with_mbedtls} )
{
	$cfg{mbedtls_src} = Cwd::realpath( $cfg{mbedtls_src} ) ;
}

# assemble qt info
if( $cfg{with_gui} && $cfg{x86} && $cfg{x64} &&
	( BuildInfo::qt_version($cfg{qt_x86}) != BuildInfo::qt_version($cfg{qt_x64}) ) )
{
	die "winbuild: error: qt version different between x86 and x64\n" ;
}
if( $cfg{with_gui} && $cfg{x86} && $cfg{x64} &&
	( BuildInfo::qt_is_static($cfg{qt_x86}) != BuildInfo::qt_is_static($cfg{qt_x64}) ) )
{
	die "winbuild: error: qt staticness different between x86 and x64\n" ;
}
my $qt_info = {
	v => BuildInfo::qt_version( $cfg{qt_x86} ? $cfg{qt_x86} : $cfg{qt_x64} ) ,
	static => $cfg{qt_static} ,
	x86 => $cfg{qt_x86} ,
	x64 => $cfg{qt_x64} ,
} ;
if( !defined($cfg{qt_static}) )
{
	$qt_info->{static} = BuildInfo::qt_is_static( $cfg{qt_x86} ? $cfg{qt_x86} : $cfg{qt_x64} ) ,
}

# project version
chomp( my $version = eval { FileHandle->new("VERSION")->gets() } || "2.7" ) ;
my $project = "emailrelay" ;

# run stuff ...
for my $part ( @cfg_run_parts )
{
	if( $part eq "configure" )
	{
		_create_winbuild_cfg() ;
	}
	if( $part eq "batchfiles" )
	{
		winbuild::spit_out_batch_files( qw(
			configure generate cmake build
			debug-build debug-test test
			mbedtls clean vclean install ) ) ;
	}
	elsif( $part eq "generate" )
	{
		run_generate( $project , $qt_info ) ;
	}
	elsif( $part eq "mbedtls" )
	{
		run_mbedtls_cmake( "x64" ) if $cfg{x64} ;
		run_mbedtls_cmake( "x86" ) if $cfg{x86} ;
		run_mbedtls_build( "x64" , "Debug" ) if ( $cfg{x64} && $cfg{debug} ) ;
		run_mbedtls_build( "x64" , "Release" ) if ( $cfg{x64} && $cfg{release} ) ;
		run_mbedtls_build( "x86" , "Debug" ) if ( $cfg{x86} && $cfg{debug} ) ;
		run_mbedtls_build( "x86" , "Release" ) if ( $cfg{x86} && $cfg{release} ) ;
	}
	elsif( $part eq "cmake" )
	{
		run_cmake( "x64" ) if $cfg{x64} ;
		run_cmake( "x86" ) if $cfg{x86} ;
	}
	elsif( $part eq "build" )
	{
		run_build( "x64" , "Release" ) if ( $cfg{x64} && $cfg{release} ) ;
		run_build( "x64" , "Debug" ) if ( $cfg{x64} && $cfg{debug} ) ;
		run_build( "x86" , "Release" ) if ( $cfg{x86} && $cfg{release} ) ;
		run_build( "x86" , "Debug" ) if ( $cfg{x86} && $cfg{debug} ) ;
	}
	elsif( $part eq "debug-build" )
	{
		run_mbedtls_cmake( "x64" ) if $cfg{with_mbedtls} ;
		run_mbedtls_build( "x64" , "Debug" ) if $cfg{with_mbedtls} ;
		run_build( "x64" , "Debug" ) ;
	}
	elsif( $part eq "clean" )
	{
		clean_test_files() ;
		run_build( "x64" , "Debug" , "clean" ) if ( $cfg{x64} && $cfg{debug} ) ;
		run_build( "x64" , "Release" , "clean" ) if ( $cfg{x64} && $cfg{release} ) ;
		run_build( "x86" , "Debug" , "clean" ) if ( $cfg{x86} && $cfg{debug} ) ;
		run_build( "x86" , "Release" , "clean" ) if ( $cfg{x86} && $cfg{release} ) ;
	}
	elsif( $part eq "vclean" )
	{
		clean_test_files() ;
		winbuild::deltree( "x64" ) ;
		winbuild::deltree( "x86" ) ;
		winbuild::deltree( mbedtls_build_dir("x64") ) ;
		winbuild::deltree( mbedtls_build_dir("x86") ) ;
		winbuild::clean_cmake_files() ;
		winbuild::clean_batch_files() ;
	}
	elsif( $part eq "debug-test" )
	{
		my $test_arch = ( $cfg{x86} && !$cfg{x64} ) ? "x86" : "x64" ;
		run_tests( "$test_arch/src/main/Debug" , "$test_arch/test/Debug" ) ;
	}
	elsif( $part eq "test" )
	{
		my $test_arch = ( $cfg{x86} && !$cfg{x64} ) ? "x86" : "x64" ;
		run_tests( "$test_arch/src/main/Release" , "$test_arch/test/Release" ) ;
	}
	elsif( $part eq "install" )
	{
		die "error: installation assembly requires building the gui" if !$cfg{with_gui} ;
		my $perl = $^X ; $perl = "$perl.exe" if ( $perl !~ m/exe$/i ) ;
		my $assembly = File::Basename::dirname($0)."/libexec/winbuild-assembly.pl" ;
		my $qt_dir = $cfg{x64} ? $qt_info->{x64} : $qt_info->{x86} ;
		die "no such qt directory [$qt_dir]" if ! -d $qt_dir ;
		my @assembly_cmd = ( $perl , $assembly ) ;
		push @assembly_cmd , "--x86" if $cfg{x86} ;
		push @assembly_cmd , "--static" if $qt_info->{static} ;
		push @assembly_cmd , ( "--qt-build-dir" , "..." ) if $qt_info->{static} ;
		push @assembly_cmd , ( "--qt-dir" , $qt_dir ) ;
		push @assembly_cmd , ( "--src-dir" , getcwd() ) ;
		push @assembly_cmd , ( "--build-dir" , getcwd()."/x64" ) ;
		print "winbuild: install: " , join(" ",@assembly_cmd) , "\n" ;
		system( @assembly_cmd ) == 0 or die ;
	}
	else
	{
		die "winbuild: usage error\n" ;
	}
}

# signal success to the batch file if we have not died
winbuild::create_touchfile( winbuild::default_touchfile($0) ) ;

# ==

sub create_cmake_files
{
	my ( $project , $qt_info ) = @_ ;

	my @makefiles = BuildInfo::read_makefiles( "." , "winbuild: reading: " ,
		{
			windows => 1 ,
			windows_mbedtls => $cfg{with_mbedtls} ,
			windows_openssl => $cfg{with_openssl} ,
			windows_gui => $cfg{with_gui} ,
			verbose => 0 ,
		}
	) ;

	$make2cmake::cfg_static_gui = $qt_info->{static} ;
	for my $m ( @makefiles )
	{
		make2cmake::create_cmake_file( $m ) ;
	}
}

sub run_generate
{
	my ( $project , $qt_info ) = @_ ;
	create_cmake_files( $project , $qt_info ) ;
}

sub run_cmake
{
	my ( $arch ) = @_ ;
	$arch ||= "x64" ;

	# assemble cmake options
	my @cmake_options = () ;
	my $cmake_arch = $arch eq "x86" ? "Win32" : $arch ;
	push @cmake_options , ( "-A" , $cmake_arch ) ;
	if( $cfg{with_mbedtls} )
	{
		my $mbedtls_build_dir = mbedtls_build_dir( $arch ) ;
		push @cmake_options , "-DMBEDTLS_INC=$mbedtls_build_dir/include" ;
		push @cmake_options , "-DMBEDTLS_RLIB=$mbedtls_build_dir/library/release" ;
		push @cmake_options , "-DMBEDTLS_DLIB=$mbedtls_build_dir/library/debug" ;
	}
	if( $cfg{with_openssl} )
	{
		my $openssl_dir = $arch eq "x64" ? $cfg{openssl_x64} : $cfg{openssl_x86} ;
		push @cmake_options , "-DOPENSSL_INC=$openssl_dir/include" ;
		push @cmake_options , "-DOPENSSL_RLIB=$openssl_dir/library/release" ;
		push @cmake_options , "-DOPENSSL_DLIB=$openssl_dir/library/debug" ;
	}
	if( $cfg{with_gui} )
	{
		my $qt_dir = defined($qt_info) ? Cwd::realpath( $qt_info->{$arch} ) : "." ;
		push @cmake_options , "-DQT_LIB=$qt_dir/lib" ;
		push @cmake_options , "-DQT_INC=$qt_dir/include" ;
		push @cmake_options , "-DQT_MOC=$qt_dir/bin/moc" ; # TODO ".exe" on windows ?
		push @cmake_options , "-DQT_VERSION=".$qt_info->{v} ;
	}
	push @cmake_options , ( "-S" , "." ) ;
	push @cmake_options , ( "-B" , $arch ) ;

	# run cmake
	print "winbuild: cmake($arch): running: [",join("][",$cfg{cmake},@cmake_options),"]\n" ;
	my $rc = system( $cfg{cmake} , @cmake_options ) ;
	print "winbuild: cmake-exit=[$rc]\n" ;
	if( $rc != 0 )
	{
		print "winbuild: cmake($arch): cmake cleanup: [$arch]\n" ;
		unlink "$arch/CMakeCache.txt" ;
		File::Path::remove_tree( "$arch/CMakeFiles" , {safe=>1,verbose=>0} ) ;
		die "winbuild: error: cmake failed\n" if $rc != 0 ;
	}
}

sub run_build
{
	my ( $arch , $confname , $target ) = @_ ;

	my @cmake_options = () ;
	push @cmake_options , ( "--build" , $arch ) ;
	push @cmake_options , ( "--target" , $target ) if $target ;
	push @cmake_options , ( "--config" , $confname ) ;
	push @cmake_options , ( "--verbose" ) if $cfg{verbose} ; # (last)

	print "winbuild: build($arch,$confname): running: [",join("][",$cfg{cmake},@cmake_options),"]\n" ;
	my $rc = system( $cfg{cmake} , @cmake_options ) ;
	print "winbuild: build($arch,$confname): exit=[$rc]\n" ;
	die unless $rc == 0 ;
}

sub mbedtls_build_dir
{
	my ( $arch ) = @_ ;
	return Cwd::realpath(dirname($0)) . "/mbedtls-$arch" ;
}

sub run_mbedtls_cmake
{
	my ( $arch ) = @_ ;

	my $src_dir = $cfg{mbedtls_src} ;
	my $build_dir = mbedtls_build_dir( $arch ) ;
	mkdir_( $build_dir ) ;

	# no-op if we already have the .sln file
	if( -f "$build_dir/mbed tls.sln" && -f "$build_dir/CMakeCache.txt" )
	{
		print "winbuild: mbedtls-cmake($arch): already got [$build_dir/mbed tls.sln]: not running cmake\n" ;
		return ;
	}

	# copy headers and edit the configuration header file
	MbedtlsBuild::copy_headers( $src_dir , $build_dir ) ;
	my $config_file = MbedtlsBuild::config_file( $build_dir ) ;
	my $enable_tls13_in_mbedtls_2x = 1 ;
	MbedtlsBuild::configure( $config_file , $enable_tls13_in_mbedtls_2x ) ;

	# assemble cmake options
	my $os = undef ;
	my $build_type = undef ; # not $confname here -- assume a multi-config generator
	my @cmake_options = MbedtlsBuild::cmake_options( $src_dir , $config_file , $arch , $os , $build_type ) ;
	push @cmake_options , ( "-B" , $build_dir ) ;
	push @cmake_options , ( "-S" , $src_dir ) ;

	# "cmake -B -S"
	print "winbuild: mbedtls-cmake($arch): running: [",join("][",$cfg{cmake},@cmake_options),"]\n" ;
	my $rc = system( $cfg{cmake} , @cmake_options ) ;
	print "winbuild: mbedtls-cmake($arch): exit=[$rc]\n" ;
	if( $rc != 0 )
	{
		print "winbuild: mbedtls-cmake($arch): cmake cleanup: [$build_dir]\n" ;
		unlink "$build_dir/CMakeCache.txt" ;
		File::Path::remove_tree( "$build_dir/CMakeFiles" , {safe=>1,verbose=>0} ) ;
		die "winbuild: error: cmake failed\n" ;
	}
}

sub run_mbedtls_build
{
	my ( $arch , $confname ) = @_ ;

	my @cmake_options = () ;
	push @cmake_options , ( "--build" , mbedtls_build_dir($arch) ) ;
	push @cmake_options , ( "--target" , "mbedtls" ) ;
	push @cmake_options , ( "--target" , "mbedcrypto" ) ;
	push @cmake_options , ( "--target" , "mbedx509" ) ;
	push @cmake_options , ( "--config" , $confname ) ;
	push @cmake_options , ( "--verbose" ) if $cfg{verbose} ; # (last)

	# "cmake --build"
	print "winbuild: mbedtls-build($arch,$confname): running: [",join("][",$cfg{cmake},@cmake_options),"]\n" ;
	my $rc = system( $cfg{cmake} , @cmake_options ) ;
	print "winbuild: mbedtls-build($arch,$confname): exit=[$rc]\n" ;
	die unless $rc == 0 ;
}

sub mkdir_
{
	my ( $dir ) = @_ ;
	return if -d $dir ;
	return mkdir( $dir ) ;
}

sub run_tests
{
	my ( $main_bin_dir , $test_bin_dir ) = @_ ;
	my $dash_v = "" ; # or "-v"
	my $script = "test/emailrelay_test.pl" ;
	my @cmd = (
		$^X , # perl
		"-Itest" , "\"$script\"" , $dash_v ,
		"-d" , "\"$main_bin_dir\"" ,
		"-x" , "\"$test_bin_dir\"" ,
		"-c" , "\"test/certificates\"" ) ;
	@cmd = grep {m/./} @cmd ;
	system( @cmd ) ;
}

sub clean_test_files
{
	my @file_list = () ;
	my @dir_list = () ;
	File::Find::find( sub { push @file_list , $File::Find::name if( -f $_ && $_ =~ m/^e\./ ) } , "." ) ;
	File::Find::find( sub { push @dir_list , $File::Find::name if $_ =~ m/^e\..*\.spool$/ } , "." ) ;
	unlink( @file_list ) ;
	map { winbuild::deltree($_) } @dir_list ;
}

sub winbuildall
{
	my $prefix = File::Basename::basename($0) ;
	my %opt = () ;
	GetOptions( \%opt , "all" , "gui=s" , "no-gui" , "qt-version=s" , "config=s" , "arch=s" , "cmake:s" ) ||
		die "$prefix: usage error\n" ;
	my $cfg_gui = $opt{gui} ? 1 : 0 ;
	$cfg_gui = 0 if $opt{'no-gui'} ;
	( my $cmake = $opt{cmake} || cmake::pick() ) =~ s/"//g ;

	my $qt_version = $opt{'qt-version'} || 6 ;
	my $src_dir = $ARGV[0] ;
	my $build_dir = $ARGV[1] ;
	-e "$src_dir/src/glib/gdef.h" or die ;

	# generate cmake files in the build tree (!)
	my @makefiles = BuildInfo::read_makefiles( $src_dir , "winbuild-all: " ,
		{
			windows => 1 ,
			windows_mbedtls => 1 ,
			windows_openssl => 1 ,
			windows_gui => $cfg_gui ,
			verbose => 0 ,
		}
	) ;
	$make2cmake::cfg_static_gui = 1 ;
	for my $m ( @makefiles )
	{
		make2cmake::create_cmake_file( $m , $build_dir ) ;
	}

	my @vars = qw( OPENSSL_INC OPENSSL_RLIB OPENSSL_DLIB
		MBEDTLS_INC MBEDTLS_RLIB MBEDTLS_DLIB
		QT_INC QT_LIB QT_MOC ) ;
	map { $ENV{$_} or die } @vars ;

	# "cmake -B -S"
	{
		my @cmake_options = () ;
		my $cmake_arch = $opt{arch} eq "x86" ? "Win32" : $opt{arch} ;
		my $qt_dir = File::Basename::dirname( $ENV{QT_INC} ) ;
		push @cmake_options , ( "-A" , $cmake_arch ) ;
		push @cmake_options , "-DMBEDTLS_INC:PATH=$ENV{MBEDTLS_INC}" ;
		push @cmake_options , "-DMBEDTLS_RLIB:PATH=$ENV{MBEDTLS_RLIB}" ;
		push @cmake_options , "-DMBEDTLS_DLIB:PATH=$ENV{MBEDTLS_DLIB}" ;
		push @cmake_options , "-DOPENSSL_INC:PATH=$ENV{OPENSSL_INC}" ;
		push @cmake_options , "-DOPENSSL_RLIB:PATH=$ENV{OPENSSL_RLIB}" ;
		push @cmake_options , "-DOPENSSL_DLIB:PATH=$ENV{OPENSSL_DLIB}" ;
		push @cmake_options , "-DQT_LIB:PATH=$qt_dir/lib" ;
		push @cmake_options , "-DQT_INC:PATH=$qt_dir/include" ;
		push @cmake_options , "-DQT_MOC:PATH=$ENV{QT_MOC}" ;
		push @cmake_options , "-DQT_VERSION=$qt_version" ;
		push @cmake_options , "-DREAL_SOURCE_DIR:PATH=$src_dir" ;
		push @cmake_options , ( "-S" , $build_dir ) ; # sic -- keep real source tree read-only
		push @cmake_options , ( "-B" , $build_dir ) ;
		my $rc = system( $cmake , @cmake_options ) ;
		die "cmake -B -S failed: [".join("][",$cmake,@cmake_options)."]" if $rc != 0 ;
	}

	# "cmake --build"
	{
		my @cmake_options = () ;
		push @cmake_options , ( "--build" , $build_dir ) ;
		push @cmake_options , ( "--config" , $opt{config} ) if $opt{config} ;
		push @cmake_options , "--verbose" ;
		my $rc = system( $cmake , @cmake_options ) ;
		die "cmake --build failed: [".join("][",$cmake,@cmake_options)."]" if $rc != 0 ;
	}
}

