@echo off
rem
rem SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
rem SPDX-License-Identifier: GPL-3.0-or-later
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
rem This batch file should normally be in a base directory containing
rem read-only source trees called "perl-src", "libressl-src", "mbedtls-src",
rem "qt-src" and "emailrelay-src". (Use "winbuildall.bat download" to
rem download and unpack sources.) However, if this batch file is being run
rem from "<edir>/libexec" then the library source trees must be under
rem "<edir>" and their build trees under "<edir>/<arch>". If a complete
rem library build tree exists then the corresponding source tree is not
rem required.
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

rem download and unpack tarballs to cwd if requested
rem
set qt=https://download.qt.io/archive/qt/6.8/6.8.3/submodules
set qtname=src-6.8.3
perl.exe -e "exit 99" 2>NUL
if "%1"=="download" (

	rem download perl
	if not "%errorlevel%"=="99" if not exist perl-src (
		echo winbuildall: downloading perl source
		curl -L -O https://www.cpan.org/src/5.0/perl-5.38.2.tar.gz
		certutil -hashfile perl-5.38.2.tar.gz SHA256 | findstr /i "a0a31534451eb7b83c7d6594a497543a54d488bc90ca00f5e34762577f40655e" >NUL
		if errorlevel 1 (
			echo winbuildall: perl tarball checksum failed
			certutil -hashfile perl-5.38.2.tar.gz SHA256
			goto error
		)
		mkdir perl-src
		tar -m -C perl-src --strip-components=1 -xzf perl-5.38.2.tar.gz
	)

	rem download libressl
	if not exist libressl-src (
		echo winbuildall: downloading libressl source
		curl -L -O https://ftp.openbsd.org/pub/OpenBSD/LibreSSL/libressl-4.2.1.tar.gz
		certutil -hashfile libressl-4.2.1.tar.gz SHA256 | findstr /i "6d5c2f58583588ea791f4c8645004071d00dfa554a5bf788a006ca1eb5abd70b"
		if errorlevel 1 (
			echo winbuildall: libressl tarball checksum failed
			certutil -hashfile libressl-4.2.1.tar.gz SHA256
			goto error
		)
		mkdir libressl-src
		tar -m -C libressl-src --strip-components=1 -xf libressl-4.2.1.tar.gz
	)

	rem download mbedtls
	if not exist mbedtls-src\CMakeLists.txt (
		echo winbuildall: downloading mbedtls source
		curl -L -O https://github.com/Mbed-TLS/mbedtls/releases/download/mbedtls-3.6.5/mbedtls-3.6.5.tar.bz2
		certutil -hashfile mbedtls-3.6.5.tar.bz2 SHA256 | findstr /i "4a11f1777bb95bf4ad96721cac945a26e04bf19f57d905f241fe77ebeddf46d8" >NUL
		if errorlevel 1 (
			echo winbuildall: mbedtls tarball checksum failed
			certutil -hashfile mbedtls-3.6.5.tar.bz2 SHA256
			goto error
		)
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
		certutil -hashfile qtbase-everywhere-%qtname%.zip SHA256 | findstr /i "992bf7766e214a341ef793eb3665fb784787d2fd666955f5f507f4c6f1f770dd" >NUL
		if errorlevel 1 (
			echo winbuildall: qtbase tarball checksum failed
			certutil -hashfile qtbase-everywhere-%qtname%.zip SHA256
			goto error
		)
		curl -L -O %qt%/qttools-everywhere-%qtname%.zip
		certutil -hashfile qttools-everywhere-%qtname%.zip SHA256 | findstr /i "4797251aa4d04c9d9da279d8da75f6348f76cff2a75192eac7cca81f5e1b1b38" >NUL
		if errorlevel 1 (
			echo winbuildall: qttools tarball checksum failed
			certutil -hashfile qttools-everywhere-%qtname%.zip SHA256
			goto error
		)
		curl -L -O %qt%/qttranslations-everywhere-%qtname%.zip
		certutil -hashfile qttranslations-everywhere-%qtname%.zip SHA256 | findstr /i "2a51d6d2a143b17fb0f9a0ac5fbab67602a950447504ec8cb8e23db12ccd3beb" >NUL
		if errorlevel 1 (
			echo winbuildall: qttranslations tarball checksum failed
			certutil -hashfile qttranslations-everywhere-%qtname%.zip SHA256
			goto error
		)
		mkdir qt-src && mkdir qt-src\qtbase && mkdir qt-src\qttools && mkdir qt-src\qttranslations
		tar -m -C qt-src/qtbase --strip-components=1 -xf qtbase-everywhere-%qtname%.zip
		tar -m -C qt-src/qttools --strip-components=1 -xf qttools-everywhere-%qtname%.zip
		tar -m -C qt-src/qttranslations --strip-components=1 -xf qttranslations-everywhere-%qtname%.zip
	)

	rem download emailrelay
	if not exist Makefile.am if not exist emailrelay-src (
		echo winbuildall: downloading emailrelay source
		curl -L -O https://sf.net/projects/emailrelay/files/emailrelay/%version%/emailrelay-%version%-src.tar.gz
		mkdir emailrelay-src
		tar -m -C emailrelay-src --strip-components=1 -xf emailrelay-%version%-src.tar.gz
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

set "basedir=%thisdir%"
set "emailrelaysrc=%basedir%emailrelay-src"
set "thisdir_=%thisdir%"
if "%thisdir:~-1%"=="\" set "thisdir_=%thisdir:~0,-1%"
if exist "%thisdir%\winbuild.pm" (
	for %%I in ("%thisdir_%") do (
		set "basedir=%%~dpI"
		set "emailrelaysrc=%%~dpI"
	)
)
if "%emailrelaysrc:~-1%"=="\" set "emailrelaysrc=%emailrelaysrc:~0,-1%"
set "perlsrc=%basedir%perl-src"
set "mbedtlssrc=%basedir%mbedtls-src"
set "libresslsrc=%basedir%libressl-src"
set "qtsrc=%basedir%qt-src"

echo winbuildall: basedir=[%basedir%]
echo winbuildall: perlsrc=[%perlsrc%]
echo winbuildall: mbedtlssrc=[%mbedtlssrc%]
echo winbuildall: libresslsrc=[%libresslsrc%]
echo winbuildall: qtsrc=[%qtsrc%]
echo winbuildall: emailrelaysrc=[%emailrelaysrc%]

if not exist "%emailrelaysrc%\src\glib\gdef.h" (
	echo winbuildall: no emailrelay source at %emailrelaysrc%
	goto error
)
if not exist "%emailrelaysrc%\src\gui\guimain.cpp" if "%gui%"=="1" (
	echo winbuildall: no emailrelay-gui source under %emailrelaysrc%\src\gui
	goto error
)
if not exist "%libresslsrc%\ssl\ssl_lib.c" (
	echo winbuildall: warning: no libressl source at %libresslsrc%
)
if not exist "%mbedtlssrc%\include\mbedtls\ssl.h" (
	echo winbuildall: warning: no mbedtls source at %mbedtlssrc%
)
if not exist "%qtsrc%\qtbase\src\corelib" if "%gui%"=="1" (
	echo winbuildall: warning: no qt source at %qtsrc%
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
if "%errorlevel%"=="99" (
	set perl=perl.exe
) else (
	if not exist "%perlsrc%\win32" (
		echo winbuildall: no perl source at %perlsrc%
		goto error
	)
	mkdir "%basedir%perl-bin" 2>NUL
	if not exist "%basedir%perl-bin\bin\perl.exe" (
		if not exist perl-build (
			echo winbuildall: copying perl source to perl-build
			mkdir perl-build
			xcopy /E /Q /V "%perlsrc%" perl-build\
		)
		echo winbuildall: building perl: CCTYPE=%cctype% INST_DRV=%thisdrive% INST_TOP=%basedir%perl-bin
		cd %perlsrc%\win32 && nmake CCTYPE=%cctype% INST_DRV=%thisdrive% "INST_TOP=%basedir%perl-bin" install
	)
	if not exist "%basedir%perl-bin\bin\perl.exe" (
		echo winbuildall: perl not built: [%basedir%perl-bin\bin\perl.exe]
		goto error
	)
	set perl=%basedir%perl-bin\bin\perl.exe
)
%perl% -e "exit 99" 2>NUL
if not "%errorlevel%"=="99" (
	echo winbuildall: perl [%perl%] not working
	goto error
)

rem libressl
rem
if exist "%basedir%%arch%\libressl-build-%arch%-%config%\library\%config%\ssl.lib" (
	echo winbuildall: libressl already built
) else (
	cd %basedir% && "%perl%" "%emailrelaysrc%/libexec/libresslbuild.pl" "--cmake=%cmake%" --config=%config% --arch=%arch% "%libresslsrc%" %arch%/libressl-build-%arch%-%config%
	if not exist "%basedir%%arch%\libressl-build-%arch%-%config%\library\%config%\ssl.lib" (
		echo winbuildall: libressl not built
		goto error
	)
)

rem mbedtls
rem
if exist "%basedir%%arch%\mbedtls-build-%arch%-%config%\library\%config%\mbedtls.lib" (
	echo winbuildall: mbedtls already built
) else (
	cd %basedir% && "%perl%" "%emailrelaysrc%/libexec/mbedtlsbuild.pl" "--cmake=%cmake%" --config=%config% --arch=%arch% "%mbedtlssrc%" %arch%/mbedtls-build-%arch%-%config%
	if not exist "%basedir%%arch%\mbedtls-build-%arch%-%config%\library\%config%\mbedtls.lib" (
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
	if exist "%basedir%%arch%\qt-bin-%arch%\lib\%corelib%" (
		echo winbuildall: qt%qtversion% already built
	) else (
		cd %basedir% && "%perl%" "%emailrelaysrc%/libexec/qtbuild.pl" "--cmake=%cmake%" --config=%config% --arch=%arch% "%qtsrc%" %arch%/qt-build-%arch%-%config% %arch%/qt-bin-%arch%
		if not exist "%basedir%%arch%\qt-bin-%arch%\lib\%corelib%" (
			echo winbuildall: qt not built
			goto error
		)
	)
)

rem emailrelay
rem
set OPENSSL_INC=%basedir%%arch%\libressl-build-%arch%-%config%\include
set OPENSSL_RLIB=%basedir%%arch%\libressl-build-%arch%-%config%\library\release
set OPENSSL_DLIB=%basedir%%arch%\libressl-build-%arch%-%config%\library\debug
set MBEDTLS_INC=%basedir%%arch%\mbedtls-build-%arch%-%config%\include
set MBEDTLS_RLIB=%basedir%%arch%\mbedtls-build-%arch%-%config%\library\release
set MBEDTLS_DLIB=%basedir%%arch%\mbedtls-build-%arch%-%config%\library\debug
set QT_INC=%basedir%%arch%\qt-bin-%arch%\include
set QT_LIB=%basedir%%arch%\qt-bin-%arch%\lib
set QT_MOC=%basedir%%arch%\qt-bin-%arch%\bin\moc.exe
"%perl%" "%emailrelaysrc%/winbuild.pl" --all --gui=%gui% --qt-version=%qtversion% "--cmake=%cmake%" --config=%config% --arch=%arch% "%emailrelaysrc%" "%basedir%%arch%\emailrelay-build-%arch%-%config%"
if not exist "%basedir%%arch%\emailrelay-build-%arch%-%config%\src\main\%config%\emailrelay.exe" (
	echo winbuildall: emailrelay executable not built
	goto error
)
if not exist "%basedir%%arch%\emailrelay-build-%arch%-%config%\src\gui\%config%\emailrelay-gui.exe" if "%gui%"=="1" (
	echo winbuildall: emailrelay gui executable not built
	goto error
)

rem assembly
rem
if "%gui%"=="1" (
	cd %basedir% && "%perl%" "%emailrelaysrc%/libexec/winbuild-assembly.pl" --static --arch=%arch% --config=%config% --src-dir "%emailrelaysrc%" --dst-dir emailrelay-windows-%arch%-%config% --build-dir %arch%/emailrelay-build-%arch%-%config% --qt-dir %arch%/qt-bin-%arch% --qt-build-dir %arch%/qt-build-%arch%-%config%
	if errorlevel 1 goto error
)

echo winbuildall: done
goto end

:error
echo winbuildall: failed
timeout /t 2

:end
@endlocal
