//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gpop_enabled.cpp
///

#include "gdef.h"
#include "gpop.h"
#include "gsecrets.h"

bool GPop::enabled() noexcept
{
	return true ;
}

std::unique_ptr<GPop::Store> GPop::newStore( const G::Path & spool_dir , const Store::Config & config )
{
	return std::make_unique<Store>( spool_dir , config ) ;
}

std::unique_ptr<GAuth::SaslServerSecrets> GPop::newSecrets( const std::string & path )
{
	return GAuth::Secrets::newServerSecrets( path , "pop-server" ) ;
}

std::unique_ptr<GPop::Server> GPop::newServer( GNet::EventState es , Store & store ,
	const GAuth::SaslServerSecrets & secrets , const Server::Config & config )
{
	return std::make_unique<Server>( es , store , secrets , config ) ;
}

void GPop::report( const Server * server , const std::string & group )
{
	if( server )
		server->report( group ) ;
}
