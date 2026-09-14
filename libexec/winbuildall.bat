@echo off
rem
rem SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
rem SPDX-License-Identifier: GPL-3.0-or-later
rem
rem Copyright (c) 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
rem
rem This program is free software: you can redistribute it and/or modify
rem it under the terms of the GNU General Public License as published by
rem the Free Software Foundation, either version 3 of the License, or
rem (at your option) any later version.
rem
rem This program is distributed in the hope that it will be useful,
rem but WITHOUT ANY WARRANTY; without even the implied warranty of
rem MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
rem GNU General Public License for more details.
rem
rem You should have received a copy of the GNU General Public License
rem along with this program.  If not, see <http://www.gnu.org/licenses/>.
rem ===
rem
rem winbuildall.bat
rem
rem Builds perl, libressl, mbedtls, Qt and emailrelay from source
rem using cmake.
rem
rem usage: winbuildall.bat [--no-gui] [{debug|release} [<arch> [<cmake>]]]
rem
rem This batch file should normally be run from a MSVC build
rem environment, ie. a "developer command prompt". Alternatively,
rem the build environment can be bootstrapped using "cmake", as
rem follows:
rem     copy emailrelay-src\etc\winbuildall-cmake.txt CMakeLists.txt
rem     cmake --fresh -A x64 .
rem     cmake --build . --config release  [runs winbuildall.bat]
rem
rem If the "cmake" path is not specified on the command-line then
rem the perl scripts called by this batch file will choose the first
rem cmake program on the PATH that has some version of Visual Studio
rem as its default generator.
rem
rem The emailrelay build produces statically-linked executables in:
rem     <arch>/emailrelay-build-<arch>-<config>/src/{main,gui}/<config>
rem
rem The built executables are then assembled into a release tree under:
rem     emailrelay-windows-<arch>-<config>
rem
rem This batch script assumes that the mbedtls and libressl libraries
rem end up in debug-or-release sub-directories. This is normally the
rem case but it will depend on the default cmake generator being
rem "multi-config" (eg. MSVC).
rem
rem This batch file should be in a base directory containing read-only
rem source trees called "perl-src", "libressl-src", "mbedtls-src",
rem "qt-src" and "emailrelay-src". Use "winbuildall.bat download" to
rem download and unpack sources.
rem
rem If downloading with git rather than fetching tarballs note that:
rem Qt and mbedtls have git submodules that must be initialised; if
rem deleting Qt ".git" sub-directories to save space then they should
rem be replaced with empty files; and some Qt git branches will not
rem include machine-generated source files, so it can be better to
rem checkout released tags rather than development branches.
rem

setlocal
set version=2.7
set thisdir=%~dp0
set thisdrive=%~d0

rem download tarballs if requested
rem
set qt=https://download.qt.io/archive/qt/6.8/6.8.3/submodules
set qtname=src-6.8.3
if "%1"=="download" (

	rem download perl
	if not exist perl-src (
		echo winbuildall: downloading perl source
		curl -L -O https://www.cpan.org/src/5.0/perl-5.38.2.tar.gz
		mkdir perl-src
		tar -m -C perl-src --strip-components=1 -xzf perl-5.38.2.tar.gz
	)

	rem download libressl
	if not exist libressl-src (
		echo winbuildall: downloading libressl source
		curl -L -O https://ftp.openbsd.org/pub/OpenBSD/LibreSSL/libressl-4.2.1.tar.gz
		mkdir libressl-src
		tar -m -C libressl-src --strip-components=1 -xf libressl-4.2.1.tar.gz
	)

	rem download mbedtls
	if not exist mbedtls-src\CMakeLists.txt (
		echo winbuildall: downloading mbedtls source
		curl -L -O https://github.com/Mbed-TLS/mbedtls/releases/download/mbedtls-3.6.5/mbedtls-3.6.5.tar.bz2
		mkdir mbedtls-src
		tar -m -C mbedtls-src --strip-components=1 -xf mbedtls-3.6.5.tar.bz2
		if not exist mbedtls-src\CMakeLists.txt (
			rmdir /q /s mbedtls-src
			cmake -E tar -xjf mbedtls-3.6.5.tar.bz2
			move mbedtls-3.6.5 mbedtls-src
		)
	)

	rem download qt
	if not exist qt-src (
		echo winbuildall: downloading Qt source
		curl -L -O %qt%/qtbase-everywhere-%qtname%.zip
		curl -L -O %qt%/qttools-everywhere-%qtname%.zip
		curl -L -O %qt%/qttranslations-everywhere-%qtname%.zip
		mkdir qt-src && mkdir qt-src\qtbase && mkdir qt-src\qttools && mkdir qt-src\qttranslations
		tar -m -C qt-src/qtbase --strip-components=1 -xf qtbase-everywhere-%qtname%.zip
		tar -m -C qt-src/qttools --strip-components=1 -xf qttools-everywhere-%qtname%.zip
		tar -m -C qt-src/qttranslations --strip-components=1 -xf qttranslations-everywhere-%qtname%.zip
	)

	rem download emailrelay
	if not exist emailrelay-src (
		echo winbuildall: downloading emailrelay source
		curl -L -O https://sf.net/projects/emailrelay/files/emailrelay/%version%/emailrelay-%version%-src.tar.gz
		mkdir emailrelay-src
		tar -m -C emailrelay-src --strip-components=1 emailrelay-%version%-src.tar.gz
	)
	goto end
)

set gui=1
set config=release
set arch=%Platform%
set cmake=
:parse
if "%~1"=="--no-gui" (set gui=0 & shift & goto parse)
if "%~1"=="Debug" set config=debug
if not "%~2"=="" set "arch=%~2"
if not "%~3"=="" set "cmake=%~3"

echo winbuildall: arch=[%arch%]
echo winbuildall: config=[%config%]
echo winbuildall: cmake=[%cmake%]

set emailrelaysrc=%thisdir%emailrelay-src
set perlsrc=%thisdir%perl-src
set mbedtlssrc=%thisdir%mbedtls-src
set libresslsrc=%thisdir%libressl-src
set qtsrc=%thisdir%qt-src

if not exist "%emailrelaysrc%\src\glib\gdef.h" (
	echo winbuildall: no emailrelay source at %emailrelaysrc%
	goto error
)
if not exist "%emailrelaysrc%\src\gui\guimain.cpp" if "%gui%"=="1" (
	echo winbuildall: no emailrelay-gui source under %emailrelaysrc%\src\gui
	goto error
)
if not exist "%libresslsrc%\ssl\ssl_lib.c" (
	echo winbuildall: no libressl source at %libresslsrc%
	goto error
)
if not exist "%mbedtlssrc%\include\mbedtls\ssl.h" (
	echo winbuildall: no mbedtls source at %mbedtlssrc%
	goto error
)
if not exist "%qtsrc%\qtbase\src\corelib" if "%gui%"=="1" (
	echo winbuildall: no qt source at %qtsrc%
	goto error
)
if not "%arch%" == "x64" (
	if not "%arch%" == "x86" (
		echo winbuildall: warning: unknown architecture [%arch%]: try running from a developer command prompt
		goto error
	)
)

rem perl
rem
set cctype=MSVC143
if "%VisualStudioVersion%" == "16.0" set cctype=MSVC142
perl.exe -e "exit 99" 2>NUL
if %errorlevel% == 99 (
	set perl=perl.exe
) else (
	if not exist "%perlsrc%\win32" (
		echo winbuildall: no perl source at %perlsrc%
		goto error
	)
	mkdir "%thisdir%perl-bin" 2>NUL
	if not exist "%thisdir%perl-bin\bin\perl.exe" (
		if not exist perl-build (
			echo winbuildall: copying perl source to perl-build
			mkdir perl-build
			xcopy /E /Q /V "%perlsrc%" perl-build\
		)
		echo winbuildall: building perl: CCTYPE=%cctype% INST_DRV=%thisdrive% INST_TOP=%thisdir%perl-bin
		cd %perlsrc%\win32 && nmake CCTYPE=%cctype% INST_DRV=%thisdrive% "INST_TOP=%thisdir%perl-bin" install
	)
	if not exist "%thisdir%perl-bin\bin\perl.exe" (
		echo winbuildall: perl not built: [%thisdir%perl-bin\bin\perl.exe]
		goto error
	)
	set perl=%thisdir%perl-bin\bin\perl.exe
)
%perl% -e "exit 99" 2>NUL
if not %errorlevel% == 99 (
	echo winbuildall: perl [%perl%] not working
	goto error
)

rem libressl
rem
if exist "%thisdir%%arch%\libressl-build-%arch%-%config%\library\%config%\ssl.lib" (
	echo winbuildall: libressl already built
) else (
	cd %thisdir% && "%perl%" "%emailrelaysrc%/libexec/libresslbuild.pl" "--cmake=%cmake%" --config=%config% --arch=%arch% "%libresslsrc%" %arch%/libressl-build-%arch%-%config%
	if not exist "%thisdir%%arch%\libressl-build-%arch%-%config%\library\%config%\ssl.lib" (
		echo winbuildall: libressl not built
		goto error
	)
)

rem mbedtls
rem
if exist "%thisdir%%arch%\mbedtls-build-%arch%-%config%\library\%config%\mbedtls.lib" (
	echo winbuildall: mbedtls already built
) else (
	cd %thisdir% && "%perl%" "%emailrelaysrc%/libexec/mbedtlsbuild.pl" "--cmake=%cmake%" --config=%config% --arch=%arch% "%mbedtlssrc%" %arch%/mbedtls-build-%arch%-%config%
	if not exist "%thisdir%%arch%\mbedtls-build-%arch%-%config%\library\%config%\mbedtls.lib" (
		echo winbuildall: mbedtls not built
		goto error
	)
)

rem qt
rem
set qtversion=6
if exist "%qtsrc%/qtbase/qtbase.pro" set qtversion=5
set corelib=Qt%qtversion%Widgets.lib
if "%config%"=="debug" set corelib=Qt%qtversion%Widgetsd.lib
if "%gui%"=="1" (
	if exist "%thisdir%%arch%\qt-bin-%arch%\lib\%corelib%" (
		echo winbuildall: qt%qtversion% already built
	) else (
		cd %thisdir% && "%perl%" "%emailrelaysrc%/libexec/qtbuild.pl" "--cmake=%cmake%" --config=%config% --arch=%arch% "%qtsrc%" %arch%/qt-build-%arch%-%config% %arch%/qt-bin-%arch%
		if not exist "%thisdir%%arch%\qt-bin-%arch%\lib\%corelib%" (
			echo winbuildall: qt not built
			goto error
		)
	)
)

rem emailrelay
rem
set OPENSSL_INC=%thisdir%%arch%\libressl-build-%arch%-%config%\include
set OPENSSL_RLIB=%thisdir%%arch%\libressl-build-%arch%-%config%\library\release
set OPENSSL_DLIB=%thisdir%%arch%\libressl-build-%arch%-%config%\library\debug
set MBEDTLS_INC=%thisdir%%arch%\mbedtls-build-%arch%-%config%\include
set MBEDTLS_RLIB=%thisdir%%arch%\mbedtls-build-%arch%-%config%\library\release
set MBEDTLS_DLIB=%thisdir%%arch%\mbedtls-build-%arch%-%config%\library\debug
set QT_INC=%thisdir%%arch%\qt-bin-%arch%\include
set QT_LIB=%thisdir%%arch%\qt-bin-%arch%\lib
set QT_MOC=%thisdir%%arch%\qt-bin-%arch%\bin\moc.exe
"%perl%" "%emailrelaysrc%/winbuild.pl" --all --gui=%gui% --qt-version=%qtversion% "--cmake=%cmake%" --config=%config% --arch=%arch% "%emailrelaysrc%" "%thisdir%%arch%\emailrelay-build-%arch%-%config%"
if not exist "%thisdir%%arch%\emailrelay-build-%arch%-%config%\src\main\%config%\emailrelay.exe" (
	echo winbuildall: emailrelay executable not built
	goto error
)
if not exist "%thisdir%%arch%\emailrelay-build-%arch%-%config%\src\gui\%config%\emailrelay-gui.exe" if "%gui%"=="1" (
	echo winbuildall: emailrelay gui executable not built
	goto error
)

rem assembly
rem
if "%gui%"=="1" (
	cd %thisdir% && "%perl%" "%emailrelaysrc%/libexec/winbuild-assembly.pl" --static --arch=%arch% --config=%config% --src-dir "%emailrelaysrc%" --dst-dir emailrelay-windows-%arch%-%config% --build-dir %arch%/emailrelay-build-%arch%-%config% --qt-dir %arch%/qt-bin-%arch% --qt-build-dir %arch%/qt-build-%arch%-%config%
	if errorlevel 1 goto error
)

echo winbuildall: done
goto end

:error
echo winbuildall: failed
timeout /t 2

:end
@endlocal
