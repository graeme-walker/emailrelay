//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file glisteners.cpp
///

#include "gdef.h"
#include "glisteners.h"
#include "glog.h"
#include "gassert.h"
#include <algorithm>

GNet::Listeners::Listeners( const Interfaces & if_ , const G::StringArray & listener_list , const std::vector<unsigned> & ports )
{
	// listeners are file-descriptors, network addresses or interface names (possibly
	// decorated with "-ipvx") -- these are parsed into fds(), fixedAddresses() and
	// interfaceAddresses() respectively
	//
	for( const auto & listener : listener_list )
	{
		int fd = G::is_windows() ? -1 : parseFd( listener ) ;
		if( fd >= 0 )
		{
			m_fds.push_back( fd ) ;
		}
		else if( isAddress(listener) && !isSpecialInterface(listener) )
		{
			for( unsigned int port : ports )
			{
				m_fixed_addresses.push_back( address(listener,port) ) ;
				if( m_fixed_addresses.back().family() == Address::Family::local )
					break ;
			}
		}
		else if( !isValidInterface(listener) )
		{
			m_invalid_names.push_back( listener ) ;
		}
		else
		{
			std::size_t n = if_.addresses( m_interface_addresses , basename(listener) , ports , af(listener) ) ;
			if( n == 0U )
				m_empty_names.push_back( listener ) ;
			else
				m_interface_names.push_back( listener ) ;
		}
	}
	if( listener_list.empty() ) // was empty()
		addWildcards( ports ) ;
}

int GNet::Listeners::af( const std::string & s )
{
	if( G::Str::tailMatch(s,"-ipv6") )
		return AF_INET6 ;
	else if( G::Str::tailMatch(s,"-ipv4") )
		return AF_INET ;
	else
		return AF_UNSPEC ;
}

std::string GNet::Listeners::basename( const std::string & s )
{
	return
		G::Str::tailMatch(s,"-ipv6") || G::Str::tailMatch(s,"-ipv4") ?
			s.substr( 0U , s.length()-5U ) :
			s ;
}

int GNet::Listeners::parseFd( const std::string & listener )
{
    if( listener.size() > 3U && listener.find("fd#") == 0U && G::Str::isUInt(listener.substr(3U)) )
    {
        int fd = G::Str::toInt( listener.substr(3U) ) ;
        if( fd < 0 ) throw InvalidFd( listener ) ;
        return fd ;
    }
    return -1 ;
}

void GNet::Listeners::addWildcards( const std::vector<unsigned> & ports )
{
	for( auto port : ports )
	{
		if( StreamSocket::supports(Address::Family::ipv4) )
			m_fixed_addresses.emplace_back( Address::Family::ipv4 , port ) ;

		if( StreamSocket::supports(Address::Family::ipv6) )
			m_fixed_addresses.emplace_back( Address::Family::ipv6 , port ) ;
	}
}

bool GNet::Listeners::isAddress( const std::string & s )
{
	return Address::validStrings( s , "0" ) ;
}

GNet::Address GNet::Listeners::address( const std::string & s , unsigned int port )
{
	return Address::parse( s , port ) ;
}

bool GNet::Listeners::empty() const
{
	return m_fds.empty() && m_fixed_addresses.empty() && m_interface_addresses.empty() ;
}

bool GNet::Listeners::defunct() const
{
	return empty() && !Interfaces::active() ;
}

bool GNet::Listeners::idle() const
{
	return empty() && !m_empty_names.empty() && Interfaces::active() ;
}

bool GNet::Listeners::noUpdates() const
{
	return !m_interface_names.empty() && !Interfaces::active() ;
}

bool GNet::Listeners::isSpecialInterface( const std::string & name )
{
	// treat "/dev/foo" on bsd as an interface not a unix domain socket
	return G::is_bsd() && G::Str::headMatch( name , "/dev/" ) && name.size() > 5U ;
}

bool GNet::Listeners::isValidInterface( const std::string & name )
{
	if( name.empty() || !Interfaces::supported() )
		return false ;
	else if( isSpecialInterface( name ) )
		return true ;
	else if( name.find('/') != std::string::npos )
		return false ; // don't allow any slashes (esp. CIDR)
	else
		return true ;
}

std::string GNet::Listeners::logString( const G::StringArray & names )
{
	return std::string(names.size()==1U?" \"":"s \"").append(G::Str::join("\", \"",names)).append(1U,'"') ;
}

G::StringArray GNet::Listeners::invalidNames() const
{
	return m_invalid_names ;
}

G::StringArray GNet::Listeners::interfaceNames() const
{
	return m_interface_names ;
}

G::StringArray GNet::Listeners::emptyNames() const
{
	return m_empty_names ;
}

const std::vector<int> & GNet::Listeners::fds() const
{
	return m_fds ;
}

const std::vector<GNet::Address> & GNet::Listeners::fixedAddresses() const
{
	return m_fixed_addresses ;
}

const std::vector<GNet::Address> & GNet::Listeners::interfaceAddresses() const
{
	return m_interface_addresses ;
}

