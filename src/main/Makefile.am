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

# force symbol stripping on 'make install'
AM_INSTALL_PROGRAM_FLAGS=-s

# change the local-state directory from .../var to .../var/spool/emailrelay
localstatedir = ${prefix}/var/spool/emailrelay

EXTRA_DIST=gmessagestore_win32.cpp emailrelay.dsp empty_file doxygen.cfg
INCLUDES = -I$(top_srcdir)/lib/gcc2.95 -I$(top_srcdir)/src/glib -I$(top_srcdir)/src/gnet
sbin_PROGRAMS = emailrelay 
libexec_PROGRAMS = emailrelay-poke
localstate_DATA = empty_file
emailrelay_SOURCES = gadminserver.cpp \
	gclientprotocol.cpp \
	gmessagestore.cpp \
	gmessagestore_unix.cpp \
	gprotocolmessage.cpp \
	gserverprotocol.cpp \
	gsmtpclient.cpp \
	gsmtpserver.cpp \
	mailrelay.cpp \
	gadminserver.h \
	gclientprotocol.h \
	gmessagestore.h \
	gprotocolmessage.h \
	gserverprotocol.h \
	gsmtp.h \
	gsmtpclient.h \
	gsmtpserver.h
emailrelay_poke_SOURCES=poke.c
emailrelay_LDADD = $(top_builddir)/src/glib/libglib.a $(top_builddir)/src/gnet/libgnet.a

