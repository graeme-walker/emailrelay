#
## SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
## SPDX-License-Identifier: GPL-3.0-or-later
#

noinst_LIBRARIES = libgssl.a

EXTRA_LIBRARIES = libgsslextra.a

AM_CPPFLAGS = -I$(top_srcdir)/src/glib

# =

if GCONFIG_TLS_USE_NONE

SSL_SOURCES = \
	gssl.h \
	gssl_none.cpp

SSL_EXTRA_SOURCES = \
	gssl.cpp \
	gssl_openssl.cpp \
	gssl_mbedtls.cpp \
	gssl_use_both.cpp \
	gssl_use_openssl.cpp \
	gssl_use_mbedtls.cpp

SSL_EXTRA_DIST = \
	gssl.cpp \
	gssl_openssl.cpp \
	gssl_openssl.h \
	gssl_mbedtls.cpp \
	gssl_mbedtls.h \
	gssl_mbedtls_headers.h \
	gssl_mbedtls_utils.h \
	gssl_use_both.cpp \
	gssl_use_openssl.cpp \
	gssl_use_mbedtls.cpp

endif

# =

if GCONFIG_TLS_USE_BOTH

SSL_SOURCES = \
	gssl.h \
	gssl.cpp \
	gssl_openssl.cpp \
	gssl_openssl.h \
	gssl_mbedtls.cpp \
	gssl_mbedtls.h \
	gssl_mbedtls_headers.h \
	gssl_mbedtls_utils.h \
	gssl_use_both.cpp

SSL_EXTRA_SOURCES = \
	gssl_none.cpp \
	gssl_use_openssl.cpp \
	gssl_use_mbedtls.cpp

SSL_EXTRA_DIST = \
	gssl_none.cpp \
	gssl_use_openssl.cpp \
	gssl_use_mbedtls.cpp

endif

# =

if GCONFIG_TLS_USE_OPENSSL

SSL_SOURCES = \
	gssl.h \
	gssl.cpp \
	gssl_openssl.cpp \
	gssl_openssl.h \
	gssl_use_openssl.cpp

SSL_EXTRA_SOURCES = \
	gssl_none.cpp \
	gssl_mbedtls.cpp \
	gssl_use_mbedtls.cpp \
	gssl_use_both.cpp

SSL_EXTRA_DIST = \
	gssl_none.cpp \
	gssl_mbedtls.cpp \
	gssl_mbedtls.h \
	gssl_mbedtls_headers.h \
	gssl_mbedtls_utils.h \
	gssl_use_mbedtls.cpp \
	gssl_use_both.cpp

endif

# =

if GCONFIG_TLS_USE_MBEDTLS

SSL_SOURCES = \
	gssl.h \
	gssl.cpp \
	gssl_mbedtls.cpp \
	gssl_mbedtls.h \
	gssl_mbedtls_headers.h \
	gssl_mbedtls_utils.h \
	gssl_use_mbedtls.cpp

SSL_EXTRA_SOURCES = \
	gssl_none.cpp \
	gssl_openssl.cpp \
	gssl_use_openssl.cpp \
	gssl_use_both.cpp

SSL_EXTRA_DIST = \
	gssl_none.cpp \
	gssl_openssl.cpp \
	gssl_openssl.h \
	gssl_use_openssl.cpp \
	gssl_use_both.cpp

endif

EXTRA_DIST = \
	$(SSL_EXTRA_DIST)

libgssl_a_SOURCES = \
	$(SSL_SOURCES)

libgsslextra_a_SOURCES = \
	$(SSL_EXTRA_SOURCES)

extra-local: libgsslextra.a

empty.cpp:
	-touch empty.cpp

CLEANFILES = empty.cpp

