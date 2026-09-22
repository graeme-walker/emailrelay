//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gsaslserverfactory.h
///

#ifndef G_SASL_SERVER_FACTORY_H
#define G_SASL_SERVER_FACTORY_H

#include "gdef.h"
#include "gsaslserver.h"
#include "gsecrets.h"
#include "goptional.h"
#include <utility>
#include <memory>

namespace GAuth
{
	class SaslServerFactory ;
}

//| \class GAuth::SaslServerFactory
/// Provides a factory function for SaslServer instances.
///
class GAuth::SaslServerFactory
{
public:
	static std::unique_ptr<SaslServer> newSaslServer( const SaslServerSecrets & ,
		bool allow_pop , const std::string & config ,
		const std::string & challenge_domain ) ;
			///< A factory function for a SaslServer.

public:
	SaslServerFactory() = delete ;
} ;

#endif
