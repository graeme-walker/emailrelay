//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gdnsbl_enabled.cpp
///

#include "gdef.h"
#include "gdnsbl.h"
#include "gdnsblock.h"
#include <utility>

class GNet::DnsblImp : private DnsBlockCallback
{
public:
	DnsblImp( std::function<void(bool)> callback , EventState es , std::string_view config ) :
		m_callback(std::move(callback)) ,
		m_block(*this,es,config)
	{
	}
	void onDnsBlockResult( const DnsBlockResult & result ) override
	{
		result.log() ;
		result.warn() ;
		m_callback( result.allow() ) ;
	}
	std::function<void(bool)> m_callback ;
	DnsBlock m_block ;
} ;

GNet::Dnsbl::Dnsbl( std::function<void(bool)> callback , EventState es , std::string_view config ) :
	m_imp(std::make_unique<DnsblImp>(callback,es,config))
{
}

GNet::Dnsbl::~Dnsbl()
= default ;

void GNet::Dnsbl::start( const Address & address )
{
	m_imp->m_block.start( address ) ;
}

bool GNet::Dnsbl::busy() const
{
	return m_imp->m_block.busy() ;
}

void GNet::Dnsbl::checkConfig( const std::string & config )
{
	DnsBlock::checkConfig( config ) ;
}

