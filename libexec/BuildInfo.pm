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
# BuildInfo.pm
#
# Extracts and enhances information read from the emailrelay
# automake makefiles.
#
# Synopis:
#
#  use BuildInfo ;
#  my @m = BuildInfo::read_makefiles( "./emailrelay" , "reading: " ,
#      {windows=>1,windows_gui=>1,windows_mbedtls=>1,windows_openssl=>0} ) ;
#  for my $m ( @m )
#  {
#    print $m->{e_whatever} , "\n" ;
#  }
#  Build::dump( @m ) ;
#

use strict ;
use Cwd ;
use File::Basename ;
use File::Glob ;
use IO::File ;
use Data::Dumper ;
use AutoMakeParser ;
use ConfigStatus ;
$AutoMakeParser::debug = 0 ;

package BuildInfo ;

sub read_makefiles
{
	my ( $base , $prefix , $opt_in ) = @_ ;

	$base ||= "." ;
	$prefix ||= "reading: " ;
	$opt_in ||= {} ;

	my $opt_for_windows = defined($opt_in->{windows}) ? $opt_in->{windows} : ( $^O =~ m/win/i ) ;
	my $opt_windows_mbedtls = $opt_in->{windows_mbedtls} ;
	my $opt_windows_openssl = $opt_in->{windows_openssl} ;
	my $opt_windows_gui = $opt_in->{windows_gui} ;
	my $opt_verbose_parser = $opt_in->{verbose} ;

	my $cs = new ConfigStatus() ; # switches() and vars()
	if( $opt_for_windows )
	{
		my %switches = windows_switches( $opt_windows_mbedtls , $opt_windows_openssl , $opt_windows_gui ) ;
		my %vars = windows_vars() ;
		$cs->load( \%switches , \%vars , "" ) ;
	}
	else
	{
		$cs->parse( ConfigStatus::find($base) ) ;
		$cs->varsref()->{top_srcdir} = sub { AutoMakeParser::up_to_base($_[0]) } ;
		$cs->varsref()->{top_builddir} = sub { AutoMakeParser::up_to_base($_[0]) } ;

		my $cspath = ConfigStatus::find( $base , "src/gui" , 1 ) ;
		if( defined($cspath) )
		{
			$cs->parse( $cspath , "src/gui" ) ;
			$cs->varsref("src/gui")->{top_srcdir} = sub { AutoMakeParser::up_to_base($_[0],-2) } ;
			$cs->varsref("src/gui")->{top_builddir} = sub { AutoMakeParser::up_to_base($_[0],-2) } ;
		}
	}
	my $cfg_with_mbedtls = $cs->switch("GCONFIG_TLS_USE_MBEDTLS") || $cs->switch("GCONFIG_TLS_USE_BOTH") ;
	my $cfg_with_openssl = $cs->switch("GCONFIG_TLS_USE_OPENSSL") || $cs->switch("GCONFIG_TLS_USE_BOTH") ;
	my $cfg_enable_gui = $cs->switch("GCONFIG_ENABLE_GUI") ;

	my @m = AutoMakeParser::readall( $base , $cs , {verbose=>$opt_verbose_parser,prefix=>$prefix,strict=>1} ) ;
	my $first = 1 ;
	my $qt_version ;
	map { $qt_version = $_->value("GCONFIG_QT_VERSION") if !$qt_version } @m ;
	$qt_version ||= 0 ;
	for my $m ( @m )
	{
		# enhance the AutoMakeParser structure with "e_whatever" fields
		$m->{e_project} = "emailrelay" ;
		$m->{e_is_windows} = $opt_for_windows ;
		$m->{e_with_mbedtls} = $cfg_with_mbedtls ; # 'with' => project-wide
		$m->{e_with_openssl} = $cfg_with_openssl ;
		$m->{e_dir} = File::Basename::dirname( $m->path() ) ;
		$m->{e_dirname} = File::Basename::basename( $m->{e_dir} ) ;
		$m->{e_rdir} = $m->rdir() ;
		$m->{e_depth} = $m->depth() ;
		$m->{e_is_top_dir} = $first ; $first = 0 ;
		$m->{e_is_gui_dir} = ( $m->path() =~ m:gui: ) ;
		$m->{e_with_qt} = $cfg_enable_gui && $m->{e_is_gui_dir} ;
		$m->{e_qt_version} = $qt_version ;
		$m->{e_cmake_out} = join( "/" , grep{/./} ( $m->{e_rdir} , "CMakeLists.txt" ) ) ;
		$m->{e_qmake_out} = $m->depth() ? join( "/" , grep{/./} ( $m->{e_rdir} , "$$m{e_dirname}.pro" ) ) : "$$m{e_project}.pro" ;
		if( $opt_for_windows )
		{
			$m->{e_compile_stdcxx} = "c++17" ;
			my @compile_options = () ;
			push @compile_options , ( "/std:c++17" , "/Zc:__cplusplus" ) ; # for Qt6
			push @compile_options , "/permissive-" ; # for Qt6 -- see qcompilerdetection.h(1326)
			my @windows7 = (
				"/DWINVER=0x0601" ,
				"/D_WIN32_WINNT=0x0601" ,
				"/DNTDDI_VERSION=0x06010000" ,
				"/D_WIN32_IE=0x0800" ) ;
			$m->{e_compile_options} = join( " " , @compile_options ) ; # use @windows7 ?
		}
		else
		{
			$m->{e_compile_options} = $m->compile_options() ;
			$m->{e_compile_options} .= " -std=c++11" if ( $m->{e_compile_options} !~ m/std=c++/ ) ;
			$m->{e_compile_options} .= " -fPIC" if $m->{e_with_qt} ;
			my @stdcxx = ( "" , map { s/^-std=// ; $_ } grep { m/^-std=c++/ } split(" ",$m->{e_compile_options}) ) ;
			$m->{e_compile_stdcxx} = $stdcxx[-1] ;
		}
		$m->{e_need_mbedtls_inc} = $cfg_with_mbedtls && $m->{e_dirname} eq "gssl" ;
		$m->{e_need_mbedtls_libs} = undef ; # see below
		$m->{e_need_openssl_inc} = $cfg_with_openssl && $m->{e_dirname} eq "gssl" ;
		$m->{e_need_openssl_libs} = undef ; # see below
		$m->{e_need_qt_inc} = $cfg_enable_gui && $m->{e_with_qt} ;
		$m->{e_need_qt_libs} = undef ; # see below

		my @mbedtls_libnames = qw( mbedtls mbedx509 mbedcrypto ) ;
		my @mbedtls_sys_libnames = () ;
		push @mbedtls_sys_libnames , "bcrypt" if( $opt_for_windows ) ; # mbedtls v3.x

		my @openssl_libnames = qw( ssl crypto ) ;

		my @qt5_libnames_release = (
			"Qt5Widgets" ,
			"Qt5Gui" ,
			"Qt5Core" ,
			"qtmain" ,
		) ;
		my @qt5_libnames_debug = map { $_."d" } @qt5_libnames_release ;

		my @qt6_libnames_release = (
			"Qt6Widgets" ,
			"Qt6Gui" ,
			"Qt6Core" ,
			"Qt6EntryPoint" ,
		) ;
		my @qt6_libnames_debug = map { $_."d" } @qt6_libnames_release ;

		my @qt5_static_libnames_release = (
			'../plugins/styles/qwindowsvistastyle' ,
			'../plugins/platforms/qwindows' ,
			'../plugins/imageformats/qico' ,
			"Qt5EventDispatcherSupport" ,
			"Qt5FontDatabaseSupport" ,
			"Qt5ThemeSupport" ,
			"Qt5AccessibilitySupport" ,
			"Qt5WindowsUIAutomationSupport" ,
			'qtfreetype' ,
			'qtpcre2' ,
			'qtharfbuzz' ,
		) ;
		my @qt5_static_libnames_debug = map {"${_}d"} @qt5_static_libnames_release ;

		my @qt6_static_libnames_release = (
			'../plugins/styles/qmodernwindowsstyle' ,
			'../plugins/platforms/qwindows' ,
			'../plugins/imageformats/qico' ,
			'Qt6BundledFreeType' ,
			'Qt6BundledPcre2' ,
			'Qt6BundledHarfbuzz' ,
			'Qt6BundledZLIB' ,
		) ;
		my @qt6_static_libnames_debug = map {"${_}d"} @qt6_static_libnames_release ;

		my @qt5_static_sys_libnames = () ;
		my @qt6_static_sys_libnames = () ;
		if( $opt_for_windows )
		{
			push @qt5_static_sys_libnames , (
				"dwmapi" , "dwrite" , "dxgi" , "dxguid" ,
				"d2d1" , "d3d9" , "d3d11" , "imm32" , "netapi32" , "ole32" ,
				"oleaut32" , "shlwapi" , "userenv" , "uxtheme" , "version" ,
				"winmm" , "winspool" , "wtsapi32" , "gdi32" ,
			) ;
			push @qt6_static_sys_libnames , (
				"synchronization" , "authz" ,
				"shcore" , "uiautomationcore" ,
				"setupapi" , "runtimeobject" ,
				"dwmapi" , "dwrite" , "dxgi" , "dxguid" ,
				"d2d1" , "d3d9" , "d3d11" , "d3d12" , "imm32" , "netapi32" , "ole32" ,
				"oleaut32" , "shlwapi" , "userenv" , "uxtheme" , "version" ,
				"winmm" , "winspool" , "wtsapi32" , "gdi32" ,
			) ;
		}

		$m->{e_mbedtls_libnames} = \@mbedtls_libnames ;
		$m->{e_openssl_libnames} = \@openssl_libnames ;

		$m->{e_qt5_libnames_debug} = \@qt5_libnames_debug ;
		$m->{e_qt5_libnames_release} = \@qt5_libnames_release ;
		$m->{e_qt5_static_libnames_debug} = \@qt5_static_libnames_debug ;
		$m->{e_qt5_static_libnames_release} = \@qt5_static_libnames_release ;
		$m->{e_qt5_static_sys_libnames} = \@qt5_static_sys_libnames ; # additional to sys_libnames

		$m->{e_qt6_libnames_debug} = \@qt6_libnames_debug ;
		$m->{e_qt6_libnames_release} = \@qt6_libnames_release ;
		$m->{e_qt6_static_libnames_debug} = \@qt6_static_libnames_debug ;
		$m->{e_qt6_static_libnames_release} = \@qt6_static_libnames_release ;
		$m->{e_qt6_static_sys_libnames} = \@qt6_static_sys_libnames ; # additional to sys_libnames

		my @subdirs = $m->subdirs() ;
		$m->{e_subdirs} = \@subdirs ;

		my @definitions = $m->definitions() ;
		push @definitions , qw(QT_WIDGETS_LIB QT_GUI_LIB QT_CORE_LIB) if $m->{e_need_qt_inc} ;
		if( $m->{e_is_gui_dir} )
		{
			@definitions = grep{!m/G_LIB_SMALL/} @definitions ;
		}
		if( $opt_for_windows )
		{
			push @definitions , "G_WINDOWS=1" ;
			push @definitions , "GCONFIG_NO_GCONFIG_DEFS=1" ;
		}
		$m->{e_definitions} = \@definitions ;

		my $up_to_src = "" ;
		if( $m->{e_depth} >= 2 && ( $m->rdir() =~ m/^src/ ) )
		{
			$up_to_src = "../" x ($m->{e_depth}-1) ;
			$up_to_src =~ s;/$;; ;
		}

		$m->{e_includes} = asref( grep {m/./} ( $up_to_src , $m->includes() ) ) ;
		$m->{e_our_includes} = asref( grep {m;^[^/\\];} @{$m->{e_includes}} ) ;
		$m->{e_more_includes} = asref( grep {m;^[/\\];} @{$m->{e_includes}} ) ;

		if( !$cfg_enable_gui )
		{
			if( $m->{e_dirname} eq "src" )
			{
				@subdirs = grep {$_ ne "gui"} @subdirs ;
			}
		}

		my @libraries = map {my $x=$_; $x =~ s/^lib//r =~ s/\.a$//r } $m->libraries() ;
		$m->{e_libraries} = \@libraries ;
		for my $library ( @libraries )
		{
			my $dotobj = $opt_for_windows ? ".obj" : ".o" ;
			my @sources = $m->sources( "lib$library.a" ) ; # windows sic
			@sources = () if( $library =~ m/extra$/ ) ;
			my @objects = map {my $x=$_;$x=~ s/\.cp*$/$dotobj/ ;$x} @sources ;

			$m->{e_library} ||= {} ;
			$m->{e_library}->{$library} ||= {} ;
			$m->{e_library}->{$library}->{libfile} = $opt_for_windows ? "$library.lib" : "lib$library.a" ;
			$m->{e_library}->{$library}->{libname} = $library ;
			$m->{e_library}->{$library}->{sources} = \@sources ;
			$m->{e_library}->{$library}->{objects} = \@objects ;
			$m->{e_library}->{$library}->{compile_options} = $m->{e_compile_options} ;
			$m->{e_library}->{$library}->{includes} = $m->{e_includes} ;
			$m->{e_library}->{$library}->{our_includes} = $m->{e_our_includes} ;
			$m->{e_library}->{$library}->{more_includes} = $m->{e_more_includes} ;
			$m->{e_library}->{$library}->{definitions} = \@definitions ;
			$m->{e_library}->{$library}->{need_mbedtls_inc} = $m->{e_need_mbedtls_inc} ;
			$m->{e_library}->{$library}->{need_openssl_inc} = $m->{e_need_openssl_inc} ;
			$m->{e_library}->{$library}->{need_qt_inc} = $m->{e_need_qt_inc} ;
		}

		my @programs = $m->programs() ;
		$m->{e_programs} = \@programs ;
		for my $program ( @programs )
		{
			my $mkey = $program ; # fwiw
			my $dotobj = $opt_for_windows ? ".obj" : ".o" ;
			my $dotexe = $opt_for_windows ? ".exe" : "" ;
			my @sources = grep {$_!~m/\.mc$/} grep {$_!~m/\.rc$/} $m->sources( $mkey ) ;
			my @objects = map {my $x=$_;$x=~ s/\.cp*$/$dotobj/ ;$x} @sources ;
			my @our_libnames = $m->our_libnames( $mkey ) ;
			my @our_libdirs = $m->our_libdirs( $mkey ) ;
			my @sys_libnames = # comctl32, pam etc
				grep {!(m/^mbed/)}
				grep {!(m/^ssl|^crypto/)}
				grep {!(m/^Qt/i)}
				$m->sys_libs( $mkey ) ;
			unshift @sys_libnames , @mbedtls_sys_libnames if $cfg_with_mbedtls ;

			my ( $mcfile , $rcfile , $binfile , $uac_type , $commoncontrols_extra , $manifest ) ;
			if( $mkey eq "emailrelay" )
			{
				$mcfile = "messages.mc" ;
				$rcfile = "emailrelay.rc" ;
				$binfile = "MSG00001.bin" ;
				$uac_type = "level='asInvoker' uiAccess='false'" ;
				$commoncontrols_extra = 0 ; # not needed -- pragma in src/win32/gcontrol.cpp
				$manifest = undef ; # not needed
			}
			elsif( $mkey eq "emailrelay-gui" )
			{
				$rcfile = "emailrelay-gui.rc" ;
				$uac_type = "level='highestAvailable' uiAccess='false'" ;
			}
			elsif( $mkey eq "emailrelay-service" )
			{
				$uac_type = "level='requireAdministrator' uiAccess='false'" ;
			}

			my $need_mbedtls_libs = $cfg_with_mbedtls && scalar( grep{$_ eq "gssl"} @our_libnames ) ;
			$m->{e_need_mbedtls_libs} ||= $need_mbedtls_libs ;
			my $need_openssl_libs = $cfg_with_openssl && scalar( grep{$_ eq "gssl"} @our_libnames ) ;
			$m->{e_need_openssl_libs} ||= $need_openssl_libs ;
			my $need_qt_libs = $cfg_enable_gui && $m->{e_is_gui_dir} && ( $program ne "emailrelay-keygen" ) ;
			$m->{e_need_qt_libs} ||= $need_qt_libs ;

			my @moc_out = $m->value( "MOC_OUTPUT" ) ;
			my @moc_in = map {my $x=$_;$x=$x =~ s/\.cpp$/.h/r =~ s/^moc_//r ;$x} @moc_out ;
			my @link_options = $opt_for_windows ? ("/MAP") : $m->link_options() ;
			my $uac_option = ( $uac_type ? "\"/MANIFESTUAC:$uac_type\"" : "/MANIFESTUAC:NO" ) ;
			my $commoncontrols_option = '"' .
				"/MANIFESTDEPENDENCY:type='win32' name='Microsoft.Windows.Common-Controls' " .
				"version='6.0.0.0' publicKeyToken='6595b64144ccf1df' language='*' processorArchitecture='*' " .
				'"'
				if $commoncontrols_extra ;

			my @our_libpairs = () ;
			for my $i ( 0 .. scalar(@our_libnames)-1 )
			{
				push @our_libpairs , [ $our_libdirs[$i] , $our_libnames[$i] ] ;
			}

			$m->{e_program} ||= {} ;
			$m->{e_program}->{$program} ||= {} ;
			$m->{e_program}->{$program}->{progfile} = "${program}${dotexe}" ;
			$m->{e_program}->{$program}->{progname} = $program ;
			$m->{e_program}->{$program}->{sources} = \@sources ;
			$m->{e_program}->{$program}->{objects} = \@objects ;
			$m->{e_program}->{$program}->{our_libpairs} = \@our_libpairs ;
			$m->{e_program}->{$program}->{our_libnames} = \@our_libnames ;
			$m->{e_program}->{$program}->{our_libdirs} = \@our_libdirs ;
			$m->{e_program}->{$program}->{need_mbedtls_libs} = $need_mbedtls_libs ; # add mbedtls_libnames
			$m->{e_program}->{$program}->{need_mbedtls_inc} = ( $program eq "emailrelay-keygen" ) ;
			$m->{e_program}->{$program}->{need_openssl_libs} = $need_openssl_libs ; # add openssl_libnames
			$m->{e_program}->{$program}->{need_openssl_inc} = undef ;
			$m->{e_program}->{$program}->{need_qt_libs} = $need_qt_libs ; # add $qt{56}_libnames_debug/release
			$m->{e_program}->{$program}->{sys_libnames} = \@sys_libnames ;
			$m->{e_program}->{$program}->{subsystem} = ($program eq "emailrelay" || $program eq "emailrelay-gui") ? "windows" : "console" ;
			$m->{e_program}->{$program}->{mcfile} = $opt_for_windows ? $mcfile : "" ;
			$m->{e_program}->{$program}->{rcfile} = $opt_for_windows ? $rcfile : "" ;
			$m->{e_program}->{$program}->{binfile} = $opt_for_windows ? $binfile : "" ;
			$m->{e_program}->{$program}->{moc_in} = \@moc_in ;
			$m->{e_program}->{$program}->{moc_out} = \@moc_out ;
			$m->{e_program}->{$program}->{link_options} = \@link_options ;
			$m->{e_program}->{$program}->{compile_options} = $m->{e_compile_options} ;
			$m->{e_program}->{$program}->{includes} = $m->{e_includes} ;
			$m->{e_program}->{$program}->{our_includes} = $m->{e_our_includes} ;
			$m->{e_program}->{$program}->{more_includes} = $m->{e_more_includes} ;
			$m->{e_program}->{$program}->{definitions} = $m->{e_definitions} ;
			$m->{e_program}->{$program}->{uac_option} = $uac_option if $opt_for_windows ;
			$m->{e_program}->{$program}->{commoncontrols_option} = $commoncontrols_option if $opt_for_windows ;
			$m->{e_program}->{$program}->{manifest} = $manifest if $opt_for_windows ;
		}
	}
	return @m ;
}

sub asref
{
	my @args = @_ ;
	return \@args ;
}

sub qt_version
{
	my ( $qt_root ) = @_ ;
	return undef if !$qt_root ;
	my $fh = new IO::File( "$qt_root/include/QtCore/qtcoreversion.h" ) ;
	my $s = eval { local $/ ; <$fh> } ;
	return 5 if ( $s =~ m/#define +QTCORE_VERSION_STR +.5/m ) ;
	return 6 if ( $s =~ m/#define +QTCORE_VERSION_STR +.6/m ) ;
	return undef ;
}

sub qt_is_static
{
	my ( $qt_root ) = @_ ;
	return undef if !$qt_root ;
	my @lib_glob = File::Glob::bsd_glob( "$qt_root/lib/Qt*Core.dll" ) ;
	my @bin_glob = File::Glob::bsd_glob( "$qt_root/bin/Qt*Core.dll" ) ;
	return (scalar(@lib_glob)+scalar(@bin_glob)) == 0 ;
}

sub dump
{
	my ( @m ) = @_ ;
	# prune leaving only "e_..." members
	for my $m ( @m )
	{
		for my $k ( keys %$m )
		{
			$m->{$k} = 'not dumped' if( $k !~ m/e_/ ) ;
		}
	}
	dumpall( @m ) ;
}

sub dumpall
{
	my ( @m ) = @_ ;
	my $d = new Data::Dumper( \@m ) ;
	$d->Sortkeys( 1 ) ;
	$d->Deepcopy( 1 ) ;
	print $d->Dump() , "\n" ;
}

sub windows_switches
{
	my ( $mbedtls , $openssl , $gui ) = @_ ;
	return (
		GCONFIG_BSD => 0 ,
		GCONFIG_DNSBL => 1 ,
		GCONFIG_EPOLL => 0 ,
		GCONFIG_GETTEXT => 0 ,
		GCONFIG_ENABLE_GUI => ($gui?1:0) ,
		GCONFIG_INSTALL_HOOK => 0 ,
		GCONFIG_INTERFACE_NAMES => 1 ,
		GCONFIG_MAC => 0 ,
		GCONFIG_PAM => 0 ,
		GCONFIG_POP => 1 ,
		GCONFIG_ADMIN => 1 ,
		GCONFIG_TESTING => 1 ,
		GCONFIG_TLS_USE_MBEDTLS => (($mbedtls&&!$openssl)?1:0) ,
		GCONFIG_TLS_USE_OPENSSL => ((!$mbedtls&&$openssl)?1:0) ,
		GCONFIG_TLS_USE_BOTH => (($mbedtls&&$openssl)?1:0) ,
		GCONFIG_TLS_USE_NONE => ((!$mbedtls&&!$openssl)?1:0) ,
		GCONFIG_UDS => 0 ,
		GCONFIG_WINDOWS => 1 ,
	) ;
}

sub windows_vars
{
	my ( $adjustment ) = @_ ;
	$adjustment ||= 0 ;
	return (
		top_srcdir => sub { AutoMakeParser::up_to_base($_[0],$adjustment) } ,
		top_builddir => sub { AutoMakeParser::up_to_base($_[0],$adjustment) } ,
		sbindir => "." ,
		mandir => "." ,
		localedir => "." ,
		e_spooldir => "c:/emailrelay" , # passed as -D but not used -- see src/gstore/gfilestore_win32.cpp
		e_docdir => "c:/emailrelay" ,
		e_initdir => "c:/emailrelay" ,
		e_bsdinitdir => "c:/emailrelay" ,
		e_rundir => "c:/emailrelay" ,
		e_icondir => "c:/emailrelay" ,
		e_trdir => "c:/emailrelay" ,
		e_examplesdir => "c:/emailrelay" ,
		e_libdir => "c:/emailrelay" ,
		e_pamdir => "c:/emailrelay" ,
		e_egconfdir => "c:/emailrelay" ,
		e_confdir => "c:/emailrelay" ,
		GCONFIG_WINDRES => "windres" ,
		GCONFIG_WINDMC => "mc" ,
		GCONFIG_QT_LIBS => "" ,
		GCONFIG_QT_CFLAGS => "-DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB" ,
		GCONFIG_QT_MOC => "" ,
		GCONFIG_TLS_LIBS => "-lmbedtls -lmbedx509 -lmbedcrypto" ,
		GCONFIG_STATIC_START => "" ,
		GCONFIG_STATIC_END => "" ,
		VERSION => "1.0" ,
		RPM_ARCH => "x86" ,
		RPM_ROOT => "rpm" ,
	) ;
}

1 ;
