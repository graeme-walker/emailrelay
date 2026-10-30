//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gpop_disabled.cpp
///

#include "gdef.h"
#include "gpop.h"
#include "gsecrets.h"

bool GPop::enabled() noexcept
{
	return false ;
}

std::unique_ptr<GPop::Store> GPop::newStore( const G::Path & , const Store::Config & )
{
	return {} ;
}

std::unique_ptr<GAuth::SaslServerSecrets> GPop::newSecrets( const std::string & )
{
	return {} ;
}

std::unique_ptr<GPop::Server> GPop::newServer( GNet::EventState , Store & ,
	const GAuth::SaslServerSecrets & , const Server::Config & )
{
	return {} ;
}

void GPop::report( const Server * , const std::string & )
{
}
