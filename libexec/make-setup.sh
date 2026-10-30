#!/bin/sh
#
# SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
# SPDX-License-Identifier: GPL-3.0-or-later
#
# make-setup.sh
#
# Builds a gui payload tree by running "make install" into it and adding
# a config file. The payload is processed by the gui installer.
#
# For Windows this is now done elsewhere (winbuild.pl).
#
# usage: make-setup.sh [-d] <payload>
#           -d : debug
#
# Normally run by "make payload" in the "src/gui" directory.
#

# parse the command line
debug="0" ; if test "$1" = "-d" ; then shift ; debug="1" ; fi
payload="$1"

# check the command-line
if test "$payload" = ""
then
	echo usage: `basename $0` '<payload>' >&2
	exit 2
fi
if test ! -d "$payload" -o -f "$payload/*"
then
	echo error: payload "[$payload]" is not an empty directory >&2
	exit 1
fi

# run "make install" into the payload directory
echo `basename $0`: running make install into $payload
payload_path="`cd \"$payload\" && pwd`"
( cd ../.. && make install GCONFIG_DOXYGEN=no "DESTDIR=$payload_path" ) > /dev/null 2>&1

# check the "./configure" was done by "bin/configure.sh --enable-gui --without-doxygen"
if test \
	! -d "$payload/usr/share/emailrelay" -o \
	! -f "$payload/usr/share/emailrelay/emailrelay.no.qm" -o \
	! -f "$payload/etc/emailrelay.conf" -o \
	! -f "$payload/usr/sbin/emailrelay-gui.real" -o \
	-f "$payload/usr/share/doc/emailrelay/doxygen/classes.html"
then
	echo `basename $0`: cannot see expected files: configure with \"configure.sh --enable-gui --without-doxygen\" >&2
	exit 1
fi

# clean up the "make install" output
rm -f "$payload/usr/sbin/emailrelay-gui"

# create the payload config file
cat <<EOF > "$payload/payload.cfg"
etc/pam.d/emailrelay=%dir-config%/pam.d/emailrelay
etc/init.d/emailrelay=%dir-config%/init.d/emailrelay
usr/lib/=%dir-install%/lib/
usr/sbin/=%dir-install%/sbin/
usr/share/=%dir-install%/share/
# permission fix-ups
+%dir-spool% group daemon 770 g+s
+%dir-install%/sbin/emailrelay-submit group daemon 775 g+s
EOF

