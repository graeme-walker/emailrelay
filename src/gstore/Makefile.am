#
## SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
## SPDX-License-Identifier: GPL-3.0-or-later
#

if GCONFIG_WINDOWS
OS_SOURCES=gfilestore_win32.cpp
OS_SOURCES_EXTRA_DIST=gfilestore_unix.cpp
else
OS_SOURCES=gfilestore_unix.cpp
OS_SOURCES_EXTRA_DIST=gfilestore_win32.cpp
endif

EXTRA_DIST=\
	$(OS_SOURCES_EXTRA_DIST)

AM_CPPFLAGS = \
	-I$(top_srcdir)/src/glib \
	-D "G_SPOOLDIR=$(e_spooldir)"

noinst_LIBRARIES = libgstore.a

libgstore_a_SOURCES = \
	$(OS_SOURCES) \
	genvelope.cpp \
	genvelope.h \
	gfiledelivery.cpp \
	gfiledelivery.h \
	gfilestore.cpp \
	gfilestore.h \
	gmessagedelivery.cpp \
	gmessagedelivery.h \
	gmessagestore.cpp \
	gmessagestore.h \
	gnewfile.cpp \
	gnewfile.h \
	gnewmessage.cpp \
	gnewmessage.h \
	gstoredfile.cpp \
	gstoredfile.h \
	gstoredmessage.cpp \
	gstoredmessage.h

