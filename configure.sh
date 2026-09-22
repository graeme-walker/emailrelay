#!/bin/sh
#
# SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
# SPDX-License-Identifier: GPL-3.0-or-later
#
# configure.sh
#
# A wrapper for the autoconf configure script that specifies more sensible
# directories depending on the host environment and simplifies
# cross-compilation.
#
# usage: configure.sh [{-d|-s <>|-z}] [{-o|-w|-p}] -- [<configure-options>]
#         -d           add debug compiler flags
#         -s           add sanitiser compiler flags (-s {address|undefined|memory|...})
#         -z           add size optimisation compiler flags
#         -o           cross-compile for openwrt (sdk under $HOME)
#         -w32         cross-compile for windows 32-bit with mingw-w64
#         -w64         cross-compile for windows 64-bit with mingw-w64
#         -p           cross-compile for rpi
#         -m           git-clone mbedtls v3
#
# If mbedtls source is found when cross-compiling then the source is
# patched (if necessary) and configured and then cmake build instructions
# are printed.
#

thisdir="`cd \`dirname $0\` && pwd`"

usage="[{-d|-s <>}] [{-o|-w32|-w64|-p}] -- <configure-args>"
while expr "x$1" : "x-" >/dev/null
do
	valued=0
	case "`echo \"$1\" | sed 's/^--*//'`" in
		m) opt_get_mbedtls=3 ;;
		d) opt_debug=1 ;;
		s) opt_sanitise="$2" ; valued=1 ;;
		z) opt_size=1 ;;
		o) opt_openwrt=1 ;;
		w32) opt_mingw=1 ; opt_win=32 ;;
		w64) opt_mingw=1 ; opt_win=64 ;;
		p) opt_rpi=1 ;;
		h) echo usage: `basename $0` $usage "..." ; $thisdir/configure --help=short ; exit 0 ;;
		#\?) echo usage: `basename $0` $usage >&2 ; exit 2 ;;
		*) opt_passthrough="$opt_passthrough $1" ;;
	esac
	if test "0$valued" -eq 1 ; then shift ; fi
	shift
done
if expr 0$opt_openwrt + 0$opt_mingw + 0$opt_rpi \> 1 > /dev/null
then
	echo usage: too many target options >&2 ; exit 2
fi

if test ! -e "$thisdir/configure"
then
	echo error: no autoconf configure script: try running \'bootstrap\' >&2
	exit 1
fi

enable_debug=""
if test "0$opt_debug" -ne 0
then
	export CFLAGS="-O0 -g"
	export CXXFLAGS="-O0 -g"
	if expr "x$*" : '.*enable.debug' >/dev/null ; then : ; else enable_debug="--enable-debug" ; fi
:
elif expr "x$*" : '.*enable.debug' >/dev/null
then
	if test "$CFLAGS$CXXFLAGS" = ""
	then
		export CFLAGS="-O0 -g"
		export CXXFLAGS="-O0 -g"
	fi
fi

if test "$opt_sanitise" != ""
then
	export CC="clang"
	export CXX="clang++"
	export CXXFLAGS="-O3 -fstrict-aliasing -Wstrict-aliasing -fsanitize=$opt_sanitise"
	export LDFLAGS="-fsanitize=$opt_sanitise"
fi

if test "0$opt_size" -eq 1
then
	export CXXFLAGS="-Os -fdata-sections -ffunction-sections"
	export LDFLAGS="-Wl,--gc-sections"
fi

MBEDTLS_DIR="`find \"$thisdir\" -maxdepth 1 -type d -name mbedtls\* ! -name mbedtls_build 2>/dev/null | head -1`"
MBEDTLS_BUILD_DIR="$MBEDTLS_DIR"
if test "$opt_get_mbedtls" != ""
then
	MBEDTLS_DIR="$thisdir/mbedtls"
	set -e
	if test -d "$thisdir/mbedtls"
	then
		git -C "$thisdir/mbedtls" fetch
	else
		git clone --recursive https://github.com/Mbed-TLS/mbedtls.git "$thisdir/mbedtls"
	fi
	git -C "$thisdir/mbedtls" checkout --recurse-submodules -q "mbedtls-3.6.7"
	set +e
fi
while test "$opt_win" != "" -o "$opt_rpi" != "" -o "$opt_openwrt" != ""
do
	if test ! -d "$MBEDTLS_DIR"
	then
		configure_mbedtls="--without-mbedtls"
		break
	fi
	set -e
	MBEDTLS_BUILD_DIR="`pwd`/mbedtls_build"
	cfg="`pwd`/mbedtls_user_config.h"
	if test -d "$MBEDTLS_BUILD_DIR" ; then : ; else mkdir "$MBEDTLS_BUILD_DIR" ; fi
	config_py="$MBEDTLS_DIR/scripts/config.py -f $cfg"
	cp $MBEDTLS_DIR/include/mbedtls/mbedtls_config.h "$cfg"
	if test "$opt_win" = "32"
	then
		# no winxp vsnprintf
		sed -i 's/^#if defined(_TRUNCATE)/#ifdef _TRUNCATE_NOT/' $MBEDTLS_DIR/library/platform.c
		# no winxp bcrypt.dll
		$config_py set MBEDTLS_NO_PLATFORM_ENTROPY
		$config_py set MBEDTLS_ENTROPY_HARDWARE_ALT
		# "must use -mpclmul -msse2 -maes` for MBEDTLS_AESNI_C"
		$config_py unset MBEDTLS_AESNI_C
	else
		sed -i 's/^#ifdef _TRUNCATE_NOT/#if defined(_TRUNCATE)/' $MBEDTLS_DIR/library/platform.c
	fi
	$config_py set MBEDTLS_SSL_PROTO_TLS1_3
	$config_py set MBEDTLS_SSL_TLS1_3_COMPATIBILITY_MODE
	if test "$opt_rpi" != ""
	then
		# "asm operand has impossible constraints or there are not enough registers"
		$config_py unset MBEDTLS_HAVE_ASM
	fi
	if test "$opt_win" = "32"
	then
		cat <<-EOF > "$MBEDTLS_BUILD_DIR/toolchain-w32.cmake"
			set(CMAKE_SYSTEM_NAME Windows)
			set(CMAKE_SYSTEM_PROCESSOR i686)
			set(TOOLCHAIN_PREFIX i686-w64-mingw32-)
			set(CMAKE_C_COMPILER \${TOOLCHAIN_PREFIX}gcc)
			set(CMAKE_CXX_COMPILER \${TOOLCHAIN_PREFIX}g++)
			set(CMAKE_RC_COMPILER \${TOOLCHAIN_PREFIX}windres)
			set(XP_FLAGS "-D_WIN32_WINNT=0x0501 -DWINVER=0x0501 -U__cpuid -UMBEDTLS_AESNI_HAVE_INTRINSICS")
			set(CMAKE_C_FLAGS "\${XP_FLAGS}" CACHE STRING "" FORCE)
			set(CMAKE_CXX_FLAGS "\${XP_FLAGS}" CACHE STRING "" FORCE)
			set(LINK_FLAGS "-Wl,--subsystem,windows:5.01")
			set(CMAKE_EXE_LINKER_FLAGS "\${LINK_FLAGS}" CACHE STRING "" FORCE)
			set(CMAKE_SHARED_LINKER_FLAGS "\${LINK_FLAGS}" CACHE STRING "" FORCE)
			set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
			set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
			set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
		EOF
	fi
	if test "$opt_win" = "64"
	then
		cat <<-EOF > "$MBEDTLS_BUILD_DIR/toolchain-w64.cmake"
			set(CMAKE_SYSTEM_NAME Windows)
			set(CMAKE_SYSTEM_PROCESSOR x86_64)
			set(TOOLCHAIN_PREFIX x86_64-w64-mingw32-)
			set(CMAKE_C_COMPILER \${TOOLCHAIN_PREFIX}gcc)
			set(CMAKE_CXX_COMPILER \${TOOLCHAIN_PREFIX}g++)
			set(CMAKE_RC_COMPILER \${TOOLCHAIN_PREFIX}windres)
			set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
			set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
			set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
		EOF
	fi
	if test "$opt_rpi" != ""
	then
		cat <<-EOF > "$MBEDTLS_BUILD_DIR/toolchain-rpi.cmake"
			set(CMAKE_SYSTEM_NAME Linux)
			set(CMAKE_SYSTEM_PROCESSOR arm)
			set(TOOLCHAIN_PREFIX arm-linux-gnueabihf-)
			set(CMAKE_C_COMPILER \${TOOLCHAIN_PREFIX}gcc)
			set(CMAKE_CXX_COMPILER \${TOOLCHAIN_PREFIX}g++)
			set(CMAKE_AR \${TOOLCHAIN_PREFIX}ar CACHE FILEPATH "Archiver")
			set(CMAKE_RANLIB \${TOOLCHAIN_PREFIX}ranlib CACHE FILEPATH "Ranlib")
			set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
			set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
			set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
		EOF
	fi
	if test "$opt_openwrt" != ""
	then
		cat <<-EOF > "$MBEDTLS_BUILD_DIR/toolchain-openwrt.cmake"
			set(CMAKE_SYSTEM_NAME Linux)
			set(CMAKE_SYSTEM_PROCESSOR __PROCESSOR__)
			set(CMAKE_C_COMPILER __CC__)
			set(CMAKE_CXX_COMPILER __CXX__)
			set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
			set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
			set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
			set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
		EOF
	fi
	configure_mbedtls="--with-mbedtls"
	export CXXFLAGS="$CXXFLAGS -I. -I../.. -DMBEDTLS_USER_CONFIG -I$MBEDTLS_DIR/include"
	export LDFLAGS="$LDFLAGS -L$MBEDTLS_BUILD_DIR/library"
	set +e
	break
done
TlsHelp()
{
	local toolchain="$1"
	if test "$toolchain" != "" ; then shift ; fi
	if test "`which gmake`" != "/usr/bin/gmake"
	then
		opt_make=" -DCMAKE_MAKE_PROGRAM=/usr/bin/make"
	fi
	opt_user_config="-DMBEDTLS_CONFIG_FILE=mbedtls_user_config.h"
	if test -d "$MBEDTLS_DIR"
	then
		if test "$toolchain" != ""
		then
			echo -n "  cmake --fresh ${opt_user_config}${opt_make} -B $MBEDTLS_BUILD_DIR -S $MBEDTLS_DIR"
			echo " --toolchain toolchain-${toolchain}.cmake" "$@"
		else
			echo "  cmake --fresh ${opt_user_config}${opt_make} -B $MBEDTLS_BUILD_DIR -S $MBEDTLS_DIR" "$@"
		fi
		echo "  cmake --build $MBEDTLS_BUILD_DIR --target lib"
	fi
}

if test "$opt_win" != ""
then
	if test "$opt_win" = "32"
	then
		TARGET="i686-w64-mingw32" # 32-bit binaries
		enable_winxp="--enable-winxp"
	else
		TARGET="x86_64-w64-mingw32" # 64-bit binaries
	fi
	export CXX="$TARGET-g++-posix"
	export CC="$TARGET-gcc-posix"
	export AR="$TARGET-ar"
	export RANLIB="$TARGET-ranlib"
	export STRIP="$TARGET-strip"
	export GCONFIG_WINDMC="$TARGET-windmc"
	export GCONFIG_WINDRES="$TARGET-windres"
	export CXXFLAGS="$CXXFLAGS -pthread"
	if test "$opt_win" = "32"
	then
		export CXXFLAGS="$CXXFLAGS -D_WIN32_WINNT=0x0501 -DWINVER=0x0501"
	fi
	export LDFLAGS="$LDFLAGS -pthread"
	if test -x "`which $CXX`" ; then : ; else echo "error: no mingw c++ compiler: [$CXX]\n" ; exit 1 ; fi
	$thisdir/configure $enable_debug --host $TARGET \
		--enable-windows $enable_winxp --disable-interface-names \
		$configure_mbedtls \
		--disable-gui --without-pam --without-doxygen \
		--prefix=/usr \
		--sysconfdir=/etc \
		--localstatedir=/var \
		e_initdir=/etc/init.d \
		$opt_passthrough "$@"
	test "$?" -eq 0 || exit 1
	echo :
	echo "build with..."
	TlsHelp w${opt_win}
	echo "  make"
	echo "  make -C src/main strip map"
	if test "$opt_win" = "32"
	then
		if test "`pwd`" = "$thisdir" ; then src="" ; more="" ; else src="$thisdir/" ; more=" --src-dir=$thisdir" ; fi
		echo "  perl ${src}libexec/winbuild-assembly.pl --winxp${more}"
		echo "  zip -r emailrelay-`cat $thisdir/VERSION`-winxp.zip emailrelay-`cat $thisdir/VERSION`-winxp"
	fi
:
elif test "$opt_rpi" != ""
then
	TARGET="arm-linux-gnueabihf"
	export CXX="$TARGET-g++"
	export CC="$TARGET-gcc"
	export AR="$TARGET-ar"
	export RANLIB="$TARGET-ranlib"
	export STRIP="$TARGET-strip"
	export CXXFLAGS="$CXXFLAGS -pthread -Wno-psabi"
	export LDFLAGS="$LDFLAGS -pthread"
	$thisdir/configure $enable_debug --host $TARGET \
		--disable-gui $configure_mbedtls --without-openssl \
		--without-pam --without-doxygen \
		--prefix=/usr \
		--sysconfdir=/etc \
		--localstatedir=/var \
		e_initdir=/etc/init.d \
		$opt_passthrough "$@"
	test "$?" -eq 0 || exit 1
	echo :
	echo "build with..."
	TlsHelp rpi
	echo "  make"
	echo "  make -C src/main strip"
:
elif test "$opt_openwrt" != ""
then
	SDK_DIR="`find $HOME -follow -maxdepth 3 -type d -iname openwrt-sdk\* 2>/dev/null | sort | head -1`"
	SDK_TOOLCHAIN_DIR="`find \"$SDK_DIR/staging_dir\" -follow -type d -iname toolchain-\* 2>/dev/null | sort | head -1`"
	SDK_TARGET_DIR="`find \"$SDK_DIR/staging_dir\" -follow -type d -iname target-\* 2>/dev/null | sort | head -1`"
	SDK_COMPILER="`find \"${SDK_TOOLCHAIN_DIR}/bin\" -follow -type f -iname \*-gcc 2>/dev/null | sort | head -1`"
	SDK_COMPILER_PREFIX="`basename \"${SDK_COMPILER}\" -gcc`"
	SDK_PROCESSOR="`echo ${SDK_COMPILER_PREFIX} | sed 's/-.*//'`"
	echo "SDK_DIR=[$SDK_DIR]"
	echo "SDK_TOOLCHAIN_DIR=[$SDK_TOOLCHAIN_DIR]"
	echo "SDK_TARGET_DIR=[$SDK_TARGET_DIR]"
	echo "SDK_COMPILER=[$SDK_COMPILER]"
	echo "SDK_PROCESSOR=[$SDK_PROCESSOR]"
	export CC="$SDK_TOOLCHAIN_DIR/bin/${SDK_COMPILER_PREFIX}-gcc"
	export CXX="$SDK_TOOLCHAIN_DIR/bin/${SDK_COMPILER_PREFIX}-c++"
	export AR="$SDK_TOOLCHAIN_DIR/bin/${SDK_COMPILER_PREFIX}-ar"
	export RANLIB="$SDK_TOOLCHAIN_DIR/bin/${SDK_COMPILER_PREFIX}-ranlib"
	export STRIP="$SDK_TOOLCHAIN_DIR/bin/${SDK_COMPILER_PREFIX}-strip"
	export CXXFLAGS="-fno-rtti -Os $CXXFLAGS"
	export LDFLAGS="$LDFLAGS -static"
	export LIBS="-lgcc_eh"
	if test -x "$CXX" ; then : ; else echo "error: no c++ compiler for target [$SDK_COMPILER_PREFIX]: CXX=[$CXX]\n" ; exit 1 ; fi
	$thisdir/configure $enable_debug --host ${SDK_COMPILER_PREFIX} \
		--disable-gui --without-pam --without-doxygen \
		$configure_mbedtls \
		--prefix=/usr \
		--sysconfdir=/etc \
		--localstatedir=/var \
		e_initdir=/etc/init.d \
		$opt_passthrough "$@"
	test "$?" -eq 0 || exit 1
	if test -d "$MBEDTLS_DIR"
	then
		sed -i "s:__CC__:$CC:" "$MBEDTLS_BUILD_DIR/toolchain-openwrt.cmake"
		sed -i "s:__CXX__:$CXX:" "$MBEDTLS_BUILD_DIR/toolchain-openwrt.cmake"
		sed -i "s:__PROCESSOR__:$SDK_PROCESSOR:" "$MBEDTLS_BUILD_DIR/toolchain-openwrt.cmake"
	fi
	echo :
	echo "build with..."
	echo "  export STAGING_DIR=\"${SDK_DIR}/staging_dir\""
	TlsHelp openwrt
	echo "  make"
	echo "  make -C src/main strip"
:
elif test "`uname`" = "NetBSD"
then
	export CXXFLAGS="$CXXFLAGS -I/usr/X11R7/include"
	export LDFLAGS="$LDFLAGS -L/usr/X11R7/lib"
	$thisdir/configure $enable_debug \
		--prefix=/usr/pkg \
		--sysconfdir=/usr/pkg/etc \
		--localstatedir=/var \
		--mandir=/usr/pkg/man \
		e_bsdinitdir=/etc/rc.d \
		$opt_passthrough "$@"
:
elif test "`uname`" = "FreeBSD"
then
	export CXXFLAGS="$CXXFLAGS -I/usr/local/include -I/usr/local/include/libav"
	export LDFLAGS="$LDFLAGS -L/usr/local/lib -L/usr/local/lib/libav"
	$thisdir/configure $enable_debug \
		--prefix=/usr/local \
		--sysconfdir=/usr/local/etc \
		--localstatedir=/usr/local/var \
		--mandir=/usr/local/man \
		e_bsdinitdir=/usr/local/etc/rc.d \
		$opt_passthrough "$@"
:
elif test "`uname`" = "OpenBSD"
then
	export CXXFLAGS="$CXXFLAGS -I/usr/X11R6/include"
	export LDFLAGS="$LDFLAGS -L/usr/X11R6/lib"
	$thisdir/configure $enable_debug \
		--prefix=/usr/local \
		--sysconfdir=/etc \
		--localstatedir=/var \
		--mandir=/usr/local/man \
		e_bsdinitdir=/usr/local/etc/rc.d \
		$opt_passthrough "$@"
:
elif test "`uname`" = "Darwin"
then
	export CXXFLAGS="$CXXFLAGS -I/opt/local/include -I/opt/X11/include"
	export LDFLAGS="$LDFLAGS -L/opt/local/lib -L/opt/X11/lib"
	$thisdir/configure $enable_debug \
		--prefix=/opt/local \
		--mandir=/opt/local/man \
		$opt_passthrough "$@"
:
elif test "`uname`" = "Linux"
then
	export CXXFLAGS
	export LDFLAGS
	$thisdir/configure $enable_debug \
		--prefix=/usr \
		--sysconfdir=/etc \
		--localstatedir=/var \
		e_initdir=/etc/init.d \
		e_systemddir=/usr/lib/systemd/system \
		e_rundir=/run/emailrelay \
		$opt_passthrough "$@"
	test "$?" -eq 0 || exit 1
	if test "$opt_get_mbedtls" != ""
	then
		echo :
		echo "build with..."
		TlsHelp
		echo "  make"
	fi
:
else
	export CXXFLAGS="$CXXFLAGS -I/usr/X11R7/include -I/usr/X11R6/include -I/usr/local/include -I/opt/local/include -I/opt/X11/include"
	export LDFLAGS="$LDFLAGS -L/usr/X11R7/lib -L/usr/X11R6/lib -L/usr/local/lib -L/opt/local/lib -L/opt/X11/lib"
	$thisdir/configure $enable_debug $opt_passthrough "$@"
fi

