#!/bin/sh
# 
# Copyright (C) 2001 Graeme Walker <graeme_walker@users.sourceforge.net>
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later
# version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
# 
# ===
#
# emailrelay.sh
# 
# A shell-script wrapper for E-MailRelay designed for
# use in the SysV-init system (/etc/init.d).
#
# usage: emailrelay.sh { start | stop }
#

# configuration
#
var_run="/var/run"
if test \! -d "${var_run}" ; then var_run="/tmp" ; fi
params=""

# initialisation
#
pid_file="${var_run}/emailrelay.pid"

# check the command line
#
usage="{ start | stop }"
if test $# -eq 0
then
	echo usage: `basename $0` "${usage}" >&2
	exit 2
fi

# process the command line
#
if test "${1}" = "start"
then
	# "start"
	#
	rm -f "${pid_file}" 2>/dev/null
	emailrelay --as-server --pid-file "${pid_file}" ${params} $@

elif test "${1}" = "stop"
then
	# "stop"
	#
	if test -f "${pid_file}" && test "`cat ${pid_file}`" != ""
	then
		kill "`cat ${pid_file}`"
	fi

else
	echo usage: `basename $0` "${usage}" >&2
	exit 2
fi

