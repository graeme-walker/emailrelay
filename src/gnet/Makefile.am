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

EXTRA_DIST=gclient_win32.cpp \
	gdescriptor_win32.cpp \
	gevent_win32.cpp \
	glocal_win32.cpp \
	grequest.cpp \
	gresolve_win32.cpp \
	gsocket_win32.cpp \
	gwinsock.cpp
INCLUDES = -I$(top_srcdir)/lib/gcc2.95 -I$(top_srcdir)/src/glib 
noinst_LIBRARIES = libgnet.a
libgnet_a_SOURCES = gaddress_ipv4.cpp \
	gclient.cpp \
	gclient_unix.cpp \
	gconnection.cpp \
	gdescriptor_unix.cpp \
	gevent.cpp \
	gevent_unix.cpp \
	geventserver.cpp \
	glinebuffer.cpp \
	glocal_unix.cpp \
	gmonitor.cpp \
	gresolve.cpp \
	gresolve_ipv4.cpp \
	gresolve_unix.cpp \
	gselect.cpp \
	gserver.cpp \
	gsocket.cpp \
	gsocket_unix.cpp \
	gtimer.cpp \
	gaddress.h \
	gclient.h \
	gconnection.h \
	gdescriptor.h \
	gevent.h \
	geventserver.h \
	glinebuffer.h \
	glocal.h \
	gmonitor.h \
	gnet.h \
	grequest.h \
	gresolve.h \
	gselect.h \
	gserver.h \
	gsocket.h \
	gtimer.h \
	gwinsock.h
