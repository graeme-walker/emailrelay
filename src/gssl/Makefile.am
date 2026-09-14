#
## Copyright (C) 2001-2024 Graeme Walker <graeme_walker@users.sourceforge.net>
## 
## This program is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
## 
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with this program.  If not, see <http://www.gnu.org/licenses/>.
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

