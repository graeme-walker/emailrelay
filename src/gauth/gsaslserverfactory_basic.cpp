//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gsaslserverfactory_basic.cpp
///

#include "gdef.h"
#include "gsaslserverfactory.h"
#include "gsaslserver.h"
#include "gsaslserverbasic.h"

std::unique_ptr<GAuth::SaslServer> GAuth::SaslServerFactory::newSaslServer( const SaslServerSecrets & secrets ,
	bool allow_pop , const std::string & config , const std::string & challenge_domain )
{
	return std::make_unique<SaslServerBasic>( secrets , allow_pop , config , challenge_domain ) ;
}

