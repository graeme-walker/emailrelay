#
## SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
## SPDX-License-Identifier: GPL-3.0-or-later
#

AM_CPPFLAGS = -I$(top_srcdir)/src/glib

noinst_LIBRARIES = libwin32.a

if GCONFIG_WINDOWS

EXTRA_DIST = empty.cpp

libwin32_a_SOURCES = \
	gappinst.cpp \
	gappinst.h \
	gappbase.cpp \
	gappbase.h \
	gcracker.cpp \
	gcracker.h \
	gdc.h \
	gdc.cpp \
	gdialog.cpp \
	gdialog.h \
	gcontrol.cpp \
	gcontrol.h \
	gpump.cpp \
	gpump.h \
	gscmap.h \
	gscmap.cpp \
	gsize.h \
	gstack.cpp \
	gstack.h \
	gtray.h \
	gtray.cpp \
	gwinbase.cpp \
	gwinbase.h \
	gwindow.cpp \
	gwindow.h

else

EXTRA_DIST = \
	gappinst.cpp \
	gappinst.h \
	gappbase.cpp \
	gappbase.h \
	gcracker.cpp \
	gcracker.h \
	gdc.h \
	gdc.cpp \
	gdialog.cpp \
	gdialog.h \
	gcontrol.cpp \
	gcontrol.h \
	gpump.cpp \
	gpump.h \
	gscmap.h \
	gscmap.cpp \
	gsize.h \
	gtray.h \
	gtray.cpp \
	gwinbase.cpp \
	gwinbase.h \
	gwindow.cpp \
	gwindow.h

libwin32_a_SOURCES = empty.cpp

endif
