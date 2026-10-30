#
## SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
## SPDX-License-Identifier: GPL-3.0-or-later
#

noinst_LIBRARIES = libgpop.a

EXTRA_LIBRARIES = libgpopextra.a

AM_CPPFLAGS = \
	-I$(top_srcdir)/src/glib \
	-I$(top_srcdir)/src/gssl \
	-I$(top_srcdir)/src/gnet \
	-I$(top_srcdir)/src/gauth \
	-Inone

if GCONFIG_POP

POP_SOURCES = \
	gpop_enabled.cpp \
	gpop.h \
	gpopserver.cpp \
	gpopserver.h \
	gpopstore.cpp \
	gpopstore.h \
	gpopserverprotocol.cpp \
	gpopserverprotocol.h

POP_EXTRA_SOURCES = \
	gpop_disabled.cpp

POP_EXTRA_DIST = \
	gpop_disabled.cpp

else

POP_SOURCES = \
	gpop_disabled.cpp \
	gpop.h

POP_EXTRA_SOURCES = \
	gpop_enabled.cpp \
	gpopserver.cpp \
	gpopstore.cpp \
	gpopserverprotocol.cpp

POP_EXTRA_DIST = \
	gpop_enabled.cpp \
	gpopserver.cpp \
	gpopserver.h \
	gpopstore.cpp \
	gpopstore.h \
	gpopserverprotocol.cpp \
	gpopserverprotocol.h

endif

EXTRA_DIST = \
	$(POP_EXTRA_DIST)

libgpop_a_SOURCES = \
	$(POP_SOURCES)

libgpopextra_a_SOURCES = \
	$(POP_EXTRA_SOURCES)

extra-local: libgpopextra.a

