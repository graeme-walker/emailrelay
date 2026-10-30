//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gsaslserverfactory_pam.cpp
///

#include "gdef.h"
#include "gsaslserverfactory.h"
#include "gexception.h"
#include "gsaslserverbasic.h"
#include "gsaslserverpam.h"

std::unique_ptr<GAuth::SaslServer> GAuth::SaslServerFactory::newSaslServer( const SaslServerSecrets & secrets ,
	bool allow_pop , const std::string & config , const std::string & challenge_domain )
{
	if( secrets.source() == "/pam" ) // deprecated
		return std::make_unique<SaslServerPam>( allow_pop ) ;
	else if( secrets.source() == "pam:" )
		return std::make_unique<SaslServerPam>( allow_pop ) ;
	else
		return std::make_unique<SaslServerBasic>( secrets , allow_pop , config , challenge_domain ) ;
}

