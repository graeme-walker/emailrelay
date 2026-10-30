#
## SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
## SPDX-License-Identifier: GPL-3.0-or-later
#

noinst_LIBRARIES = libgauth.a

EXTRA_LIBRARIES = libgauthextra.a

AM_CPPFLAGS = \
	-I$(top_srcdir)/src/glib \
	-I$(top_srcdir)/src/gssl \
	-I$(top_srcdir)/src/gnet \
	-Inone

if GCONFIG_PAM
 PAM_SOURCES = \
	gsaslserverfactory_pam.cpp \
	gsaslserverpam.cpp \
	gsaslserverpam.h
 PAM_EXTRA_SOURCES = \
	gsaslserverfactory_basic.cpp
 PAM_EXTRA_DIST = \
	gsaslserverfactory_basic.cpp
else
 PAM_SOURCES = \
	gsaslserverfactory_basic.cpp
 PAM_EXTRA_SOURCES = \
	gsaslserverfactory_pam.cpp \
	gsaslserverpam.cpp
 PAM_EXTRA_DIST = \
	gsaslserverfactory_pam.cpp \
	gsaslserverpam.cpp \
	gsaslserverpam.h
endif

EXTRA_DIST = \
	$(PAM_EXTRA_DIST)

libgauth_a_SOURCES = \
	$(PAM_SOURCES) \
	gcram.h \
	gcram.cpp \
	gsaslclient.h \
	gsaslclient.cpp \
	gsaslclientsecrets.h \
	gsaslclientsecrets.cpp \
	gsaslserver.h \
	gsaslserver.cpp \
	gsaslserversecrets.h \
	gsaslserversecrets.cpp \
	gsaslserverbasic.h \
	gsaslserverbasic.cpp \
	gsaslserverfactory.h \
	gsecret.h \
	gsecret.cpp \
	gsecrets.h \
	gsecrets.cpp \
	gsecretsfile.cpp \
	gsecretsfile.h

libgauthextra_a_SOURCES = \
	$(PAM_EXTRA_SOURCES)

extra-local: libgauthextra.a

