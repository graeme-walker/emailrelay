//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file ginterfaces_common.cpp
///

#include "gdef.h"
#include "ginterfaces.h"
#include "gstr.h"
#include "gtest.h"
#include <algorithm>

GNet::Interfaces::Interfaces( EventState es ) :
	m_es(es)
{
}

GNet::Interfaces::Interfaces( EventState es , InterfacesHandler & handler ) :
	m_es(es) ,
	m_handler(&handler)
{
}

GNet::Interfaces::~Interfaces()
= default;

void GNet::Interfaces::load()
{
	std::vector<Item> new_list ;
	loadImp( m_es , new_list ) ;
	m_loaded = true ;
	using std::swap ;
	swap( m_list , new_list ) ;
}

bool GNet::Interfaces::supported()
{
	return true ;
}

bool GNet::Interfaces::loaded() const
{
	return m_loaded ;
}

std::vector<GNet::Address> GNet::Interfaces::addresses( const std::string & name , unsigned int port , int af ) const
{
	std::vector<GNet::Address> result ;
	addresses( result , name , port , af ) ;
	return result ;
}

std::size_t GNet::Interfaces::addresses( std::vector<Address> & out , const std::string & name ,
	unsigned int port , int af ) const
{
	return addresses( out , name , std::vector<unsigned>{port} , af ) ;
}

std::size_t GNet::Interfaces::addresses( std::vector<Address> & out , const std::string & name ,
	const std::vector<unsigned> & ports , int af ) const
{
	if( !loaded() )
		const_cast<Interfaces*>(this)->load() ;

	std::size_t count = 0U ;
	for( const auto & item : m_list )
	{
		if( !name.empty() && ( item.name == name || item.altname == name ) && item.up && item.valid_address )
		{
			if( af == AF_UNSPEC ||
				( af == AF_INET6 && item.address.is6() ) ||
				( af == AF_INET && item.address.is4() ) )
			{
				for( auto port : ports )
				{
					count++ ;
					out.push_back( item.address ) ;
					out.back().setPort( port ) ;
				}
			}
		}
	}
	return count ;
}

G::StringArray GNet::Interfaces::names( bool all ) const
{
	G::StringArray list ;
	for( const auto & iface : *this )
	{
		if( all || iface.up )
			list.push_back( iface.name ) ;
	}
	std::sort( list.begin() , list.end() ) ;
	list.erase( std::unique(list.begin(),list.end()) , list.end() ) ;
	return list ;
}

GNet::Interfaces::const_iterator GNet::Interfaces::begin() const
{
	return m_list.begin() ;
}

GNet::Interfaces::const_iterator GNet::Interfaces::end() const
{
	return m_list.end() ;
}

void GNet::Interfaces::readEvent()
{
	if( m_notifier )
	{
		std::string s = m_notifier->readEvent() ;
		if( m_handler && !s.empty() )
			m_handler->onInterfaceEvent( s ) ;
	}
}

void GNet::Interfaces::onFutureEvent()
{
	if( m_notifier )
	{
		std::string s = m_notifier->onFutureEvent() ;
		if( m_handler && !s.empty() )
			m_handler->onInterfaceEvent( s ) ;
	}
}

// ==

GNet::Interfaces::Item::Item() :
	address(Address::defaultAddress())
{
}

