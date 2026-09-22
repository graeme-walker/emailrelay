#
## SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
## SPDX-License-Identifier: GPL-3.0-or-later
#

noinst_LIBRARIES = libgfilters.a

AM_CPPFLAGS = \
	-I$(top_srcdir)/src/glib \
	-I$(top_srcdir)/src/gssl \
	-I$(top_srcdir)/src/gnet \
	-I$(top_srcdir)/src/win32 \
	-I$(top_srcdir)/src/gstore \
	-I$(top_srcdir)/src/gsmtp

libgfilters_a_SOURCES = \
	gcopyfilter.cpp \
	gcopyfilter.h \
	gdeliveryfilter.cpp \
	gdeliveryfilter.h \
	gexecutablefilter.cpp \
	gexecutablefilter.h \
	gfilterchain.cpp \
	gfilterchain.h \
	gfilterfactory.cpp \
	gfilterfactory.h \
	gmessageidfilter.cpp \
	gmessageidfilter.h \
	gmxfilter.cpp \
	gmxfilter.h \
	gmxlookup.cpp \
	gmxlookup.h \
	gnetworkfilter.cpp \
	gnetworkfilter.h \
	gnullfilter.cpp \
	gnullfilter.h \
	gsimplefilterbase.cpp \
	gsimplefilterbase.h \
	gspamfilter.cpp \
	gspamfilter.h \
	gsplitfilter.cpp \
	gsplitfilter.h

