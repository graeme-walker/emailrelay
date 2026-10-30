#
## SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
## SPDX-License-Identifier: GPL-3.0-or-later
#

noinst_LIBRARIES = libgverifiers.a

AM_CPPFLAGS = \
	-I$(top_srcdir)/src/glib \
	-I$(top_srcdir)/src/gssl \
	-I$(top_srcdir)/src/gnet \
	-I$(top_srcdir)/src/win32 \
	-I$(top_srcdir)/src/gstore \
	-I$(top_srcdir)/src/gsmtp

libgverifiers_a_SOURCES = \
	gexecutableverifier.cpp \
	gexecutableverifier.h \
	gnetworkverifier.cpp \
	gnetworkverifier.h \
	ginternalverifier.cpp \
	ginternalverifier.h \
	guserverifier.cpp \
	guserverifier.h \
	gverifierfactory.cpp \
	gverifierfactory.h

