#
## SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
## SPDX-License-Identifier: GPL-3.0-or-later
#

AM_CPPFLAGS = \
	-I$(top_srcdir)/src/glib \
	-I$(top_srcdir)/src/gssl \
	-I$(top_srcdir)/src/gnet \
	-I$(top_srcdir)/src/gauth \
	-I$(top_srcdir)/src/gstore \
	-Inone

noinst_LIBRARIES = libgsmtp.a

EXTRA_LIBRARIES = libgsmtpextra.a

if GCONFIG_ADMIN
 ADMIN_SOURCES = gadminserver.h gadminserver_enabled.cpp
 ADMIN_EXTRA_DIST = gadminserver_disabled.cpp
 ADMIN_EXTRA_SOURCES = gadminserver.h gadminserver_disabled.cpp
else
 ADMIN_SOURCES = gadminserver.h gadminserver_disabled.cpp
 ADMIN_EXTRA_DIST = gadminserver_enabled.cpp
 ADMIN_EXTRA_SOURCES = gadminserver.h gadminserver_enabled.cpp
endif

EXTRA_DIST = \
	$(ADMIN_EXTRA_DIST)

libgsmtp_a_SOURCES = \
	$(ADMIN_SOURCES) \
	grequestclient.cpp \
	grequestclient.h \
	gspamclient.cpp \
	gspamclient.h \
	gfilter.cpp \
	gfilter.h \
	gfilterfactorybase.cpp \
	gfilterfactorybase.h \
	gprotocolmessage.cpp \
	gprotocolmessageforward.cpp \
	gprotocolmessageforward.h \
	gprotocolmessage.h \
	gprotocolmessagestore.cpp \
	gprotocolmessagestore.h \
	gsmtpclient.cpp \
	gsmtpclient.h \
	gsmtpclientprotocol.cpp \
	gsmtpclientprotocol.h \
	gsmtpclientreply.cpp \
	gsmtpclientreply.h \
	gsmtpforward.cpp \
	gsmtpforward.h \
	gsmtpserver.cpp \
	gsmtpserver.h \
	gsmtpserverbufferin.cpp \
	gsmtpserverbufferin.h \
	gsmtpserverflowcontrol.h \
	gsmtpserverparser.cpp \
	gsmtpserverparser.h \
	gsmtpserverprotocol.cpp \
	gsmtpserverprotocol.h \
	gsmtpserversend.cpp \
	gsmtpserversend.h \
	gsmtpserversender.h \
	gsmtpservertext.cpp \
	gsmtpservertext.h \
	gverifier.cpp \
	gverifier.h \
	gverifierfactorybase.cpp \
	gverifierfactorybase.h \
	gverifierstatus.cpp \
	gverifierstatus.h

libgsmtpextra_a_SOURCES = \
	$(ADMIN_EXTRA_SOURCES)

extra-local: libgsmtpextra.a

