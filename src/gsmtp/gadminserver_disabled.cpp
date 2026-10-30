//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gadminserver_disabled.cpp
///

#include "gdef.h"
#include "gadminserver.h"

class GSmtp::AdminServerImp
{
} ;

bool GSmtp::AdminServer::enabled()
{
	return false ;
}

GSmtp::AdminServer::AdminServer( GNet::EventState , GStore::MessageStore & ,
	FilterFactoryBase & , const GAuth::SaslClientSecrets & ,
	const G::StringArray & , const Config & )
{
}

GSmtp::AdminServer::~AdminServer()
= default ;

void GSmtp::AdminServer::emitCommand( Command , unsigned int )
{
}

G::Slot::Signal<GSmtp::AdminServer::Command,unsigned int> & GSmtp::AdminServer::commandSignal()
{
	throw NotImplemented() ;
}

void GSmtp::AdminServer::report( const std::string & ) const
{
}

void GSmtp::AdminServer::notify( const std::string & , const std::string & , const std::string & , const std::string & )
{
}

GStore::MessageStore & GSmtp::AdminServer::store()
{
	throw NotImplemented() ;
}

GSmtp::FilterFactoryBase & GSmtp::AdminServer::ff()
{
	throw NotImplemented() ;
}

const GAuth::SaslClientSecrets & GSmtp::AdminServer::clientSecrets() const
{
	throw NotImplemented() ;
}

bool GSmtp::AdminServer::notifying() const
{
	return false ;
}

