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
# emailrelay-notify.sh
#
# An example script which looks for failed mail in the MailRelay spool
# directory, and sends failure notification messages using 'procmail'.
#

tmp="/tmp/`basename $0`.$$.tmp"
trap "rm -f ${tmp} 2>/dev/null ; exit 0" 0 1 2 3 13 15

# parse the command line
#
store="/var/spool/mailrelay"
if test $# -ge 1
then
	store="${1}"
fi

# check the spool directory
#
if test \! -d "${store}"
then
	echo `basename $0`: invalid spool directory >&2
	exit 1
fi

# for each failed e-mail...
#
for file in ${store}/mail-relay.*.envelope.bad ""
do
	if test -f "${file}"
	then
		content="`echo ${file} | sed 's/envelope/content/' | sed 's/.bad//'`"
		reason="`fgrep MailRelay-Reason ${file} | sed 's/X-[^ ]*Reason: //' | tr -d '\015'`"
		from="`fgrep MailRelay-From ${file} | sed 's/X-MailRelay-From: //' | tr -d '\015'`"
		deliver_to="${from}"
		if test "${deliver_to}" = "" 
		then 
			deliver_to="postmaster"
		fi

		# create a notification message header
		#
		echo "To: ${deliver_to}" > ${tmp}
		echo "From: postmaster" >> ${tmp}
		echo "Subject: Your e-mail could not be delivered" >> ${tmp}
		echo " " >> ${tmp}

		# add the message content
		#
		if test "${reason}" != ""
		then
			echo "Reason: ${reason}" >> ${tmp}
		fi
		if test -f "${content}"
		then
			egrep -i '^To:|^Subject:' ${content} >> ${tmp}
			echo " " >> ${tmp}
			echo "The original mail is saved as \"${content}\"." >> ${tmp}
			echo "You should make a copy of this file if necessary and then delete it." >> ${tmp}
		fi

		# deliver the notification using procmail
		#
		echo `basename $0`: delivering `basename ${content}` to ${deliver_to}
		procmail -d ${deliver_to} < ${tmp}
		rc=$?
		if test "${rc}" -eq 0
		then
			rm -f "${file}" 2>/dev/null
		fi
	fi
done

