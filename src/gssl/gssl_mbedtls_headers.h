//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gssl_mbedtls_headers.h
///

#ifndef G_SSL_MBEDTLS_HEADERS_H
#define G_SSL_MBEDTLS_HEADERS_H

#include "gdef.h"

#include <mbedtls/version.h>
#if MBEDTLS_VERSION_MAJOR >= 3
#include <mbedtls/build_info.h>
#endif
#include <psa/crypto.h>
#include <mbedtls/ssl_ciphersuites.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/error.h>
#include <mbedtls/pem.h>
#include <mbedtls/base64.h>
#include <mbedtls/debug.h>
#include <mbedtls/md5.h>
#include <mbedtls/sha1.h>
#include <mbedtls/sha256.h>
#include <mbedtls/sha512.h>
#if GCONFIG_HAVE_MBEDTLS_NET_H
#include <mbedtls/net.h>
#else
#include <mbedtls/net_sockets.h>
#endif

#endif
