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
# emailrelay-deliver.sh
#
# An example script which looks for local mail in the MailRelay 
# spool directory, and delivers it using 'procmail'.
#

store="/var/spool/mailrelay"
postmaster="root"

# parse the command line
#
if test $# -ge 1
then
	store="${1}"
fi

# check the spool directory is valid
#
if test \! -d "${store}"
then
	echo `basename $0`: invalid spool directory >&2
	exit 1
fi

# for each e-mail to a local recipient...
#
for file in ${store}/mail-relay.*.envelope.local ""
do
	if test -f "${file}"
	then
		content="`echo ${file} | sed 's/envelope/content/'`"

		deliver_to="`fgrep X-MailRelay-LocalTo ${file} | sed 's/X-MailRelay-LocalTo: //' | tr -d '\015' | sed \"s/postmaster/${postmaster}/g\"`"
		if test "${deliver_to}" = "" 
		then 
			deliver_to="${postmaster}"
		fi

		# deliver using procmail
		#
		if test -f "${content}"
		then
			echo `basename $0`: delivering `basename ${content}` to ${deliver_to}
			procmail -d ${deliver_to} < ${content}
			rc=$?
			if test "${rc}" -eq 0
			then
				echo '' # rm -f "${file}" 2>/dev/null
			fi
		fi
	fi
done

