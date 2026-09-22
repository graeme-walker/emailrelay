#!/bin/sh
#
# SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
# SPDX-License-Identifier: GPL-3.0-or-later
#
# emailrelay_test.sh
#
# Tests the E-MailRelay system using "emailrelay_test.pl" or some other
# shell script that takes "-d <exe-dir>".
#
# usage:
#    emailrelay_test.sh [-q] [<perl-script-options>] [<test-name> ...]
#    emailrelay_test.sh --sh <test-script>
#

this_dir="`cd \`dirname \"$0\"\` && pwd`"

# run another shell script if asked
if test "$1" = "--sh"
then
	shift
	script="$1"
	shift
	buildroot="`cd .. && pwd`"
	exec "$script" -d "$buildroot" "$@"
	exit 1
fi

# test for perl
perl -e "use Carp; use FileHandle; use Getopt::Std; use IO::Socket; use IO::Select; use lib \".\" ; exit(99);" 2>/dev/null
rc=$?
if test $rc -ne 99
then
	echo `basename $0`: warning: no perl, or missing perl modules: skipping all tests >&2
	exit 77 # for automake
fi

# parse the command-line
quiet=0
if test "$1" = "-q"
then
	# no shift here - pass it on
	quiet=1
fi

# check the perl script
if test -z "$srcdir"
then
	srcdir="$this_dir"
fi
perl_script="$srcdir/emailrelay_test.pl"
chmod +x "$0" "$perl_script" 2>/dev/null
if test ! -e "$perl_script"
then
	echo `basename $0`: error: no perl script: "$perl_script" >&2
	exit 1
fi

# run the emailrelay_test.pl tests
if sh -c "true </dev/tty" 2>/dev/null
then
	# redirect stdin because "make -j" and "openssl s_server"
	perl -I"${srcdir}" "$perl_script" "$@" </dev/tty
else
	perl -I"${srcdir}" "$perl_script" "$@"
fi
rc=$?

if test "$quiet" -eq 1 -o "$rc" -eq 77 ; then : ; else
	echo `basename $0`: done: $rc
fi
exit $rc
