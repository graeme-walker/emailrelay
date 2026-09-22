//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gsaslclientsecrets.h
///

#ifndef G_SASL_CLIENT_SECRETS_H
#define G_SASL_CLIENT_SECRETS_H

#include "gdef.h"
#include "gsecret.h"
#include "gstringview.h"

namespace GAuth
{
	class SaslClientSecrets ;
}

//| \class GAuth::SaslClientSecrets
/// An interface used by GAuth::SaslClient to obtain a client id and
/// its authentication secret. Conceptually there is one client and
/// they can have secrets encoded in multiple ways.
///
class GAuth::SaslClientSecrets
{
public:
	virtual ~SaslClientSecrets() = default ;
		///< Destructor.

	virtual bool validSelector( std::string_view selector ) const = 0 ;
		///< Returns true if the selector is valid.

	virtual bool mustAuthenticate( std::string_view selector ) const = 0 ;
		///< Returns true if authentication is required.
		///< Precondition: validSelector()

	virtual Secret clientSecret( std::string_view type , std::string_view selector ) const = 0 ;
		///< Returns the client secret for the given type. The
		///< type is "plain" or the CRAM hash algorithm or "oauth".
		///< The optional selector is used to choose between
		///< available client accounts. Returns an invalid secret
		///< if none.
} ;

#endif
