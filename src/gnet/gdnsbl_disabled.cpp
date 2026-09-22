//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gdnsbl_disabled.cpp
///

#include "gdef.h"
#include "gdnsbl.h"
#include "gexception.h"

class GNet::DnsblImp
{
} ;

GNet::Dnsbl::Dnsbl( std::function<void(bool)> callback , EventState , std::string_view ) :
	m_callback(callback)
{
}

GNet::Dnsbl::~Dnsbl()
= default ;

void GNet::Dnsbl::start( const Address & )
{
	m_callback( /*allow=*/true ) ;
}

bool GNet::Dnsbl::busy() const
{
	return false ;
}

void GNet::Dnsbl::checkConfig( const std::string & config )
{
	if( !config.empty() )
		throw G::Exception( "dnsbl has been disabled in this build" ) ;
}
