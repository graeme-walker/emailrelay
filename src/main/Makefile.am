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

EXTRA_DIST=commandline_win32.cpp main_win32.cpp gmessagestore_win32.cpp emailrelay.dsp icon-32.ico icon2.ico icon3.ico icon4.ico empty_file doxygen.cfg emailrelay.rc resource.h gsasl_cyrus.cpp
INCLUDES = -I$(top_srcdir)/lib/gcc2.95 -I$(top_srcdir)/src/glib -I$(top_srcdir)/src/gnet
sbin_PROGRAMS = emailrelay 
libexec_PROGRAMS = emailrelay-poke
localstate_DATA = empty_file
emailrelay_SOURCES = \
	gadminserver.cpp \
	gadminserver.h \
	gbase64.cpp \
	gbase64.h \
	gclientprotocol.cpp \
	gclientprotocol.h \
	gfilestore.cpp \
	gfilestore.h \
	gmessagestore.cpp \
	gmessagestore.h \
	gmessagestore_unix.cpp \
	gnewfile.cpp \
	gnewfile.h \
	gnewmessage.cpp \
	gnewmessage.h \
	gprotocolmessage.cpp \
	gprotocolmessage.h \
	gprotocolmessageforward.cpp \
	gprotocolmessageforward.h \
	gprotocolmessagestore.cpp \
	gprotocolmessagestore.h \
	gsasl_login.cpp \
	gsasl.h \
	gsecrets.cpp \
	gsecrets.h \
	gserverprotocol.cpp \
	gserverprotocol.h \
	gsmtp.h \
	gsmtpclient.cpp \
	gsmtpclient.h \
	gsmtpserver.cpp \
	gsmtpserver.h \
	gstoredfile.cpp \
	gstoredfile.h \
	gstoredmessage.cpp \
	gstoredmessage.h \
	gverifier.cpp \
	gverifier.h \
	gxtext.cpp \
	gxtext.h \
	main_unix.cpp \
	configuration.cpp \
	configuration.h \
	run.cpp \
	run.h \
	commandline.cpp \
	commandline_unix.cpp \
	commandline.h
emailrelay_poke_SOURCES=poke.c
emailrelay_LDADD = $(top_builddir)/src/glib/libglib.a $(top_builddir)/src/gnet/libgnet.a

