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
# emailrelay-test.sh
#
# Test the E-MailRelay system.
#
# Creates three temporary spool directories under /tmp and runs
# three emailrelay servers to bucket-brigade a test message from one to 
# the next. The test succeeds if the message gets into the third 
# spool directory.
#
# If this test takes more than a second or two then it has failed.
#

exe="../src/main/emailrelay"
poke="../src/main/emailrelay-poke"
pp="1001" # port-prefix
base_dir="/tmp/`basename $0`.$$.tmp"
exit_code="1"

Cleanup()
{
	kill `cat ${base_dir}/pid-* 2>/dev/null` 2>/dev/null
	if test -d ${base_dir}
	then
		grep "." ${base_dir}/log-? > /tmp/`basename $0`.out 2>/dev/null
	fi
	rm -rf ${base_dir} 2>/dev/null
}

Trap() 
{ 
	Cleanup
	exit ${exit_code}
}

RunServer()
{
	port_="${1}"
	spool_="${2}"
	log_="${3}"
	pidfile_="${4}"
	extra_="${5}"

	mkdir -p ${base_dir}/${spool_}
	${exe} --log --no-syslog --port ${port_} --spool-dir ${base_dir}/${spool_} \
		--pid-file ${base_dir}/${pidfile_} ${extra_} 2> ${base_dir}/${log_}
}

RunClient()
{
	to_="${1}"
	spool_="${2}"
	log_="${3}"
	pidfile_="${4}"

	${exe} --forward --no-daemon --dont-serve --log --no-syslog \
		--pid-file ${base_dir}/${pidfile_} \
		--forward-to ${to_} --spool-dir ${base_dir}/${spool_} 2> ${base_dir}/${log_}
}

RunPoke()
{
	port_="${1}"
	log_="${2}"

	${poke} ${port_} > ${base_dir}/${log_}
}

Content()
{
	echo "To: recipient-1@localhost, recipient-2@localhost"
	echo "Subject: test message 1"
	echo "From: sender"
	echo " "
	echo "Content"
}

Envelope()
{
	echo "X-MailRelay-Format: #2821.2"
	echo "X-MailRelay-Content: 8bit"
	echo "X-MailRelay-From: sender"
	echo "X-MailRelay-ToCount: 2"
	echo "X-MailRelay-To-Remote: recipient-1@localhost"
	echo "X-MailRelay-To-Remote: recipient-2@localhost"
	echo "X-MailRelay-End: 1"
}

CrLf()
{
	sed 's/$/�/' | tr '�' '\r'
}

CheckResults()
{
	if test -f ${base_dir}/store-3/*.envelope -a -f ${base_dir}/store-3/*.content
	then
		exit_code="0"
		echo `basename $0`: succeeded
	else
		echo `basename $0`: failed: see /tmp/`basename $0`.out >&2
	fi
}

StartTimer()
{
	( sleep 5 ; Cleanup ) &
}

CreateMessages()
{
	mkdir -p ${base_dir}/store-1
	Content | CrLf > ${base_dir}/store-1/emailrelay.0.1.content
	Envelope | CrLf > ${base_dir}/store-1/emailrelay.0.1.envelope
}

trap "Trap ; exit" 1 2 3 13 15
trap "Trap 0 ; exit" 0

StartTimer
RunServer ${pp}1 store-2 log-1 pid-1
RunServer ${pp}2 store-2 log-2 pid-2 "--admin ${pp}4 --forward-to localhost:${pp}3"
RunServer ${pp}3 store-3 log-3 pid-3
CreateMessages
RunClient localhost:${pp}1 store-1 log-c pid-4
RunPoke ${pp}4 log-p
CheckResults

