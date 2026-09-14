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
# winbuild-assembly.pl
#
# Copies files from the build into a Windows release assembly ready for
# zipping and distribution. Called from "winbuild.bat" and "winbuildall.bat".
#
# usage: winbuild-assembly.pl [options]
#         --static                 static GUI build, so no windeployqt
#         --arch={x64|x86}         build architecture
#         --config={release|debug} build type
#         --dst-dir=<dir>          emailrelay install dir (eg. "emailrelay-<v>-w64")
#         --build-dir=<dir>        emailrelay build dir ("emailrelay-bin-<arch>")
#         --src-dir=<dir>          emailrelay source dir ("emailrelay-src")
#         --qt-dir=<dir>           qt install dir ("qt-bin-<arch>")
#         --qt-build-dir=<dir>     qt build dir ("qt-build-<arch>-<config>") (for --static)
#         --msvc-dir=<dir>         msvc installation directory (for windeployqt)
#         --version=<v>            project version
#         --no-keygen              no keygen executable available
#         --no-mapfiles            no mapfiles
#         --winxp                  special assembly for winxp, with no GUI
#
# The Qt deployment tool "windeployqt" is used to assemble extra
# dependencies and translations, although it only works for a
# non-static GUI build so for static GUI builds the translations
# are copied out of the Qt build tree.
#

use strict ;
use Cwd ;
use IO::File ;
use File::Find ;
use File::Basename ;
use File::Copy ;
use File::Glob ;
use File::Path ;
use Getopt::Long qw( :config no_ignore_case ) ;
#use lib dirname($0) ;

my %opt = () ;
GetOptions( \%opt ,
	"build-dir|B=s" , "src-dir|S=s" , "qt-dir|Q=s" , "qt-build-dir|X=s" , "msvc-dir|M=s" , "dst-dir|O=s" ,
	"version=s" , "arch=s" , "config=s" , "static" , "no-keygen" , "no-mapfiles" , "winxp" )
	or _die( "usage error" ) ;
my $project = "emailrelay" ;
$opt{arch} ||= "x64" ;
$opt{config} ||= "release" ;
$opt{'build-dir'} ||= "$project-bin-$opt{arch}" ;
$opt{'qt-dir'} ||= "qt-bin-$opt{arch}" ;
$opt{'qt-build-dir'} ||= "qt-build-$opt{arch}-$opt{config}" ;

if( $opt{winxp} )
{
	$opt{'src-dir'} ||= "." ;
	$opt{version} ||= _version_from_file( $opt{'src-dir'} ) ;
	$opt{'dst-dir'} ||= "$project-$opt{version}-winxp" ;
	install_winxp( $opt{'src-dir'} , "." , $opt{'dst-dir'} , $opt{version} ) ;
}
else
{
	$opt{'src-dir'} ||= "$project-src" ;
	$opt{version} ||= _version_from_file( $opt{'src-dir'} ) ;
	my $wxx = ( $opt{arch} eq "x86" ) ? "w32" : "w64" ;
	$opt{'dst-dir'} ||= "$project-$opt{version}-$wxx" ;
	my $msvc_dir = $opt{'msvc-dir'} || $ENV{VCINSTALLDIR} || _msvc_dir_from_cmake() ;

	-d $msvc_dir or _die( "no msvc base directory [$msvc_dir]" ) ;
	-d $opt{'src-dir'} or _die( "no source base directory [$opt{'src-dir'}]" ) ;
	-d $opt{'build-dir'} or _die( "no build base directory [$opt{'build-dir'}]" ) ;
	-d $opt{'qt-dir'} or _die( "no qt directory [$opt{'qt-dir'}]" ) ;
	-d $opt{'qt-build-dir'} or _die( "no qt build directory [$opt{'qt-build-dir'}]" ) if $opt{static} ;

	install( $opt{'dst-dir'} , $opt{'src-dir'} , $opt{'build-dir'} , $opt{config} , $opt{static} ,
		$opt{'qt-dir'} , $opt{'qt-build-dir'} , $msvc_dir , $opt{version} , !$opt{'no-keygen'} ) ;
}

# ==

sub install
{
	my ( $dst_dir , $src_dir , $build_dir , $config , $static , $qt_dir , $qt_build_dir , $msvc_dir , $version , $with_keygen ) = @_ ;

	_log( "dst=[$dst_dir]" ) ;
	_log( "build=[$build_dir]" ) ;
	_log( "msvc=[$msvc_dir]" ) ;
	_log( "qt=[$qt_dir]" ) ;
	_log( "qt-build=[$qt_build_dir]" ) if $static ;

	install_core( $src_dir , "$build_dir/src/main/$config" , $dst_dir , $version ) ;
	copy_file( "$build_dir/src/gui/$config/emailrelay-gui.exe" , "$dst_dir/emailrelay-setup.exe" ) ;
	create_nouac( "$dst_dir/emailrelay-setup.exe" ) ;
	copy_file( "$build_dir/src/main/$config/emailrelay-keygen.exe" , "$dst_dir/programs/" ) if $with_keygen ;
	create_payload_cfg( "$dst_dir/payload/payload.cfg" ) ;
	install_core( $src_dir , "$build_dir/src/main/$config" , "$dst_dir/payload/files" , $version , 1 ) ;
	copy_file( "$build_dir/src/gui/$config/emailrelay-gui.exe" , "$dst_dir/payload/files/gui/" ) ;
	create_nouac( "$dst_dir/payload/files/gui/emailrelay-gui.exe" ) ;
	copy_file( "$build_dir/src/main/$config/emailrelay-keygen.exe" , "$dst_dir/payload/files/programs/" ) if $with_keygen ;

	# template files are used by installer and are not in payload.cfg
	my $in = ( _old($version) || -e "$src_dir/etc/emailrelay.auth.in" ) ? ".in" : "" ;
	copy_file( "$src_dir/etc/emailrelay.auth${in}" , "$dst_dir/payload/files/installer/emailrelay.auth.in" , 1 ) ; # "-authtemplate"
	copy_file( "$src_dir/etc/emailrelay.cfg${in}" , "$dst_dir/payload/files/installer/emailrelay.cfg.in" , 1 ) ; # "-conftemplate"

	if( $static )
	{
		# no windeployqt so just copy translation files from the qttranslations
		# build -- note that "qt_fr.qm" etc are small meta-catalogues for
		# backwards compatibility for Qt4 that point to "qtbase_fr.qm" etc. --
		# in guimain.cpp we load "qtbase" files first (for static builds) and
		# then "qt" (for dynamic builds using windeployqt)
		#
		my $copy_opt = { not_match=>qr/qt_help_/ , at_least=>10 } ;
		my $from = "$qt_build_dir/qttranslations/translations" ;
		my $to1 = "$dst_dir/translations/" ;
		my $to2 = "$dst_dir/payload/files/gui/translations/" ;
		copy_files( "$from/qtbase_*.qm" , $to1 , $copy_opt ) ;
		copy_files( "$from/qtbase_*.qm" , $to2 , $copy_opt ) ;
	}
	else
	{
		# use windeployqt
		install_gui_dependencies( $qt_dir , $msvc_dir ,
			"$dst_dir/emailrelay-setup.exe" ,
			"$dst_dir/payload/files/gui/emailrelay-gui.exe" ) ;
	}

	copy_files( "$src_dir/src/gui/*.qm" , "$dst_dir/translations/" , {at_least=>1} ) ;
	copy_files( "$src_dir/src/gui/*.qm" , "$dst_dir/payload/files/gui/translations/" , {at_least=>1} ) ;

	_log( "$config assembly created in [$dst_dir]" ) ;
}

sub install_winxp
{
	my ( $src_dir , $build_dir , $dst_dir , $version ) = @_ ;
	install_core( $src_dir , "$build_dir/src/main" , $dst_dir , $version , 0 ) ;
	copy_file( "$build_dir/src/main/emailrelay-keygen.exe" , "$dst_dir/programs/emailrelay-keygen.exe" ) ;
	rename( "$dst_dir/news.txt" , "$dst_dir/doc/news.txt" ) or die ;
	rename( "$dst_dir/changelog.txt" , "$dst_dir/doc/changelog.txt" ) or die ;
	rename( "$dst_dir/authors.txt" , "$dst_dir/doc/authors.txt" ) or die ;
	rename( "$dst_dir/scripts/emailrelay-service-install.js" , "$dst_dir/emailrelay-service-install.js" ) ;
	rmdir( "$dst_dir/scripts" ) ;
	{
		my $fh = new IO::File( "$dst_dir/emailrelay-start.bat" , "w" ) or die ;
		print $fh "start \"emailrelay\" programs\\emailrelay.exe \@app/../emailrelay.cfg\r\n" ;
		$fh->close() or die ;
	}
	{
		my $fh = new IO::File( "$dst_dir/emailrelay.cfg" , "w" ) or die ;
		print $fh "#\r\n# emailrelay.cfg\r\n#\r\n\r\n" ;
		print $fh "# Use emailrelay-start.bat to run emailrelay with this config.\r\n" ;
		print $fh "# For demo purposes this only does forwarding once on startup.\r\n" ;
		print $fh "# Change 'forward' to 'forward-on-disconnect' once you\r\n" ;
		print $fh "# have set a valid 'forward-to' address.\r\n" ;
		for my $item ( qw(
			show=window,tray
			log
			verbose
			log-time
			log-file=@app/../log-%d.txt
			syslog
			close-stderr
			spool-dir=@app/..
			port=25
			interface=0.0.0.0
			forward
			forward-to=127.0.0.1:25
			pop
			pop-port=110
			pop-auth=@app/../popauth.txt
			) )
		{
			print $fh "$item\r\n" ;
		}
		print $fh "\r\n" ;
		$fh->close() or die ;
	}
	{
		my $fh = new IO::File( "$dst_dir/popauth.txt" , "w" ) or die ;
		print $fh "#\r\n# popauth.txt\r\n#\r\n" ;
		print $fh "server plain postmaster postmaster\r\n" ;
		$fh->close() or die ;
	}
	{
		my $fh = new IO::File( "$dst_dir/programs/emailrelay-service.cfg" , "w" ) or die ;
		print $fh "dir-config=\"\@app/..\"\r\n" ;
		$fh->close() or die ;
	}
	{
		my $fh = new IO::File( "$dst_dir/emailrelay-submit-test.bat" , "w" ) or die ;
		my $cmd = "\@echo off\r\n" ;
		$cmd .= "programs\\emailrelay-submit.exe -N -n -s \@app/.. --from postmaster " ;
		$cmd .= "-C U3ViamVjdDogdGVzdA== " ; # subject
		$cmd .= "-C = " ;
		$cmd .= "-C VGVzdCBtZXNzYWdl " ; # body
		$cmd .= "-d -F -t " ;
		$cmd .= "postmaster" ; # to
		print $fh "$cmd\r\n" ;
		print $fh "pause\r\n" ;
		$fh->close() or die ;
	}
}

sub install_gui_dependencies
{
	my ( $qt_dir , $msvc_dir , @exes ) = @_ ;

	$ENV{VCINSTALLDIR} = $msvc_dir ; # used by windeployqt to copy runtime files
	my $deploy_tool = "$qt_dir/bin/windeployqt.exe" ;
	for my $exe ( @exes )
	{
		my $rc = system( $deploy_tool , $exe ) ;
		$rc == 0 or _die( "failed running [$deploy_tool] [$exe]" ) ;
	}
}

sub create_payload_cfg
{
	my ( $file ) = @_ ;
	File::Path::make_path( File::Basename::dirname($file) ) ;
	my $fh = new IO::File( $file , "w" ) or _die( "cannot create [$file]" ) ;
	print $fh "files/programs/=\%dir-install\%/\n" ;
	print $fh "files/scripts/=\%dir-install\%/\n" ;
	print $fh "files/examples/=\%dir-install\%/examples/\n" ;
	print $fh "files/doc/=\%dir-install\%/doc/\n" ;
	print $fh "files/base/=\%dir-install\%/\n" ;
	print $fh "files/gui/=\%dir-install\%/\n" ;
	$fh->close() or die ;
}

sub install_core
{
	my ( $src_dir , $build_exe_dir , $out_dir , $version , $is_payload ) = @_ ;

	my $examples = "examples" ;
	if( _old($version) || !-d "$src_dir/examples" )
	{
		$examples = "bin" ;
	}

	my $base = $is_payload ? "base" : "." ;
	my %copy = qw(
		__src__/README __base__/readme.txt
		__src__/AUTHORS __base__/authors.txt
		__src__/NEWS __base__/news.txt
		__src__/ChangeLog __base__/changelog.txt
		__exe__/emailrelay-service.exe programs/
		__exe__/emailrelay.exe programs/
		__exe__/emailrelay-submit.exe programs/
		__exe__/emailrelay-passwd.exe programs/
		__exe__/emailrelay-textmode.exe programs/
		__src__/bin/emailrelay-service-install.js scripts/
		__src__/__examples__/emailrelay-*.pl examples/
		__src__/__examples__/emailrelay-*.py examples/
		__src__/__examples__/emailrelay-*.js examples/
		__src__/__examples__/emailrelay-*.sh examples/
		__src__/doc/authentication.png doc/
		__src__/doc/forwardto.png doc/
		__src__/doc/whatisit.png doc/
		__src__/doc/serverclient.png doc/
		__src__/doc/developer.txt doc/
		__src__/doc/reference.txt doc/
		__src__/doc/userguide.txt doc/
		__src__/doc/windows.txt doc/,__base__/readme-windows.txt
	) ;
	if( -d "$src_dir/LICENSES" )
	{
		$copy{"__src__/LICENSES"} = "__base__/licenses.txt" ;
		$copy{"__src__/LICENSES/FSFAP.txt"} = "__base__/license_fsfap.txt" ;
		$copy{"__src__/LICENSES/GPL-3.0-or-later.txt"} = "__base__/license_gpl3.txt" ;
	}
	else
	{
		$copy{"__src__/LICENSE"} = "__base__/license.txt" ;
	}
	if( !$is_payload && !$opt{'no-mapfiles'} )
	{
		# ("/MAP" in BuildInfo.pm)
		$copy{"__exe__/emailrelay.map"} = "build/" ;
		$copy{"__exe__/emailrelay-textmode.map"} = "build/" ;
	}
	while( my ($from,$to_list) = each %copy )
	{
		my @to = split( m/,/ , $to_list ) ;
		for my $to_in ( @to )
		{
			$from =~ s:__src__:$src_dir:g ;
			$from =~ s:__exe__:$build_exe_dir:g ;
			$from =~ s:__examples__:$examples:g ;
			( my $to = $to_in ) =~ s:__base__:$base:g ;
			$to = "" if $to eq "." ;
			copy_files( $from , "$out_dir/$to" , {at_least=>1} ) ;
		}
	}
	_fixup( $out_dir ,
		[ "$base/readme.txt" , "$base/license.txt" ] ,
		{
			README => 'readme.txt' ,
			COPYING => 'copying.txt' ,
			AUTHORS => 'authors.txt' ,
			INSTALL => 'install.txt' ,
			ChangeLog => 'changelog.txt' ,
		} ) ;
}

sub copy_file
{
	my ( $src , $dst , $to_crlf ) = @_ ;

	if( $dst =~ m:/$: )
	{
		$dst =~ s:/$:: ;
		File::Path::make_path( $dst ) ;
		-d $dst or _die( "failed to create target directory [$dst]" ) ;
	}
	elsif( ! -d File::Basename::dirname($dst) )
	{
		File::Path::make_path( File::Basename::dirname($dst) ) ;
	}

	for my $ext ( "txt" , "js" , "pl" , "pm" , "py" )
	{
		if( -d $dst )
		{
			$to_crlf = 1 if ( $src =~ m/\.${ext}$/ ) ;
		}
		else
		{
			$to_crlf = 1 if( $dst =~ m/\.${ext}$/ ) ;
		}
	}

	if( $to_crlf )
	{
		if( -d $dst ) { $dst = "$dst/".File::Basename::basename($src) }
		my $fh_in = new IO::File( $src , "r" ) ;
		my $fh_out = new IO::File( $dst , ">:raw" ) ;
		( $fh_in && $fh_out ) or _die( "failed to copy [$src] to [$dst]" ) ;
		while(<$fh_in>)
		{
			chomp( my $line = $_ ) ;
			$line =~ s/\r$// ;
			print $fh_out $line , "\r\n" ;
		}
		$fh_out->close() or _die( "failed to copy [$src] to [$dst]" ) ;
	}
	else
	{
		File::Copy::copy( $src , $dst ) or _die( "failed to copy [$src] to [$dst]" ) ;
	}
}

sub copy_files
{
	my ( $glob , $dst_dir , $opt ) = @_ ;
	$opt ||= {} ;
	my $at_least = $opt->{at_least} || 0 ;
	my @files = _glob( $glob ) ;
	if( exists($opt->{not_match}) ) { @files = grep { $_ !~ m/$$opt{not_match}/ } @files }
	_die( "no matching files [$glob]" ) if scalar(@files) == 0 ;
	_die( "too few matching files [$glob]" ) if scalar(@files) < $at_least ;
	map { copy_file( $_ , $dst_dir ) } @files ;
}

sub create_nouac
{
	my ( $exe ) = @_ ;
	my $name = File::Basename::basename( $exe ) ;
	( my $bat = $exe ) =~ s/\.exe$/-nouac.bat/ ;
	my $fh = new IO::File( $bat , "w" ) or _die( "cannot create [$bat]" ) ;
	print $fh "\@echo off\n" ;
	print $fh "set __COMPAT_LAYER=RunAsInvoker\n" ;
	print $fh ".\\$name\n" ;
	$fh->close() or die ;
}

sub _fixup
{
	my ( $base , $fnames , $fixes ) = @_ ;
	for my $fname ( @$fnames )
	{
		my $fh_in = new IO::File( "$base/$fname" , "r" ) or _die( "cannot read [$base/$fname]" ) ;
		my $fh_out = new IO::File( "$base/$fname.$$.tmp" , "w" ) or die ;
		while(<$fh_in>)
		{
			my $line = $_ ;
			for my $from ( keys %$fixes )
			{
				my $to = $fixes->{$from} ;
				$line =~ s/\Q$from\E/$to/g ;
			}
			print $fh_out $line ;
		}
		$fh_in->close() or die ;
		$fh_out->close() or die ;
		rename( "$base/$fname.$$.tmp" , "$base/$fname" ) or die ;
	}
}

sub _msvc_dir_from_cmake
{
	my $msvc_linker =
		_cmake_cache_value_msvc_linker( "x64" ) ||
		_cmake_cache_value_msvc_linker( "x86" ) ;
	my $bin_dir = File::Basename::dirname( $msvc_linker ) ;
	my ( $vc_dir ) = ( $bin_dir =~ m:(.*/vc)/.*:i ) ; # could to better
	return Cwd::realpath( $vc_dir ) ;
}

sub _cmake_cache_value_msvc_linker
{
	my ( $arch ) = @_ ;
	my $msvc_linker = _cmake_cache_value( $arch , qr/^CMAKE_LINKER:[A-Z]+=(.*)/ ) ;
	$msvc_linker or _die( "cannot read linker path from CMakeCache.txt" ) ;
	return $msvc_linker ;
}

sub _cmake_cache_value
{
	my ( $arch , $re ) = @_ ;
	my $fh = new IO::File( "$arch/CMakeCache.txt" , "r" ) or _die( "cannot open cmake cache file" ) ;
	my $value ;
	while(<$fh>)
	{
		chomp( my $line = $_ ) ;
		my ( $x ) = ( $line =~ m/$re/i ) ;
		if( $x )
		{
			$value = $x ;
			last ;
		}
	}
	return $value ;
}

sub _version_from_file
{
	my ( $src_dir ) = @_ ;
	_die( "invalid source directory [$src_dir]" ) if ! -d $src_dir ;
	my $v = eval { IO::File->new("$src_dir/VERSION")->gets() } ;
	chomp( $v ) ;
	_die( "no version from [$src_dir/VERSION]" ) if !$v ;
	return $v ;
}

sub _old
{
	return $_[0] eq "2.6" ;
}

sub _glob
{
	return File::Glob::bsd_glob( @_ ) ;
}

sub _log
{
	print "winbuild-assembly: " , @_ , "\n" ;
}

sub _die
{
	die "winbuild-assembly: error: " . join(" ",@_) . "\n" ;
}

