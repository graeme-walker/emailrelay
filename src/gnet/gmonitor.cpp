//
// Copyright (C) 2001 Graeme Walker <graeme_walker@users.sourceforge.net>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
// ===
//
// gmonitor.cpp
//

#include "gdef.h"
#include "gnet.h"
#include "gmonitor.h"
#include "gassert.h"
#include <set>

class GNet::MonitorImp
{
public:
	MonitorImp() ;
	typedef const Client * C_p ;
	typedef const ServerPeer * S_p ;
	typedef std::set<C_p GLessAllocator(C_p,C_p) > Clients ;
	typedef std::set<S_p GLessAllocator(S_p,S_p) > ServerPeers ;
	Clients m_clients ;
	ServerPeers m_server_peers ;
	unsigned long m_client_adds ;
	unsigned long m_client_removes ;
	unsigned long m_server_peer_adds ;
	unsigned long m_server_peer_removes ;
} ;

GNet::MonitorImp::MonitorImp() :
	m_client_adds(0UL) ,
	m_client_removes(0UL) ,
	m_server_peer_adds(0UL) ,
	m_server_peer_removes(0UL)
{
}

// ===

GNet::Monitor * GNet::Monitor::m_this = NULL ;

GNet::Monitor::Monitor() :
	m_imp( new MonitorImp )
{
	G_ASSERT( m_this == NULL ) ;
	m_this = this ;
}

GNet::Monitor::~Monitor()
{
	delete m_imp ;
	m_this = NULL ;
}

GNet::Monitor * GNet::Monitor::instance()
{
	return m_this ;
}

void GNet::Monitor::add( const Client & client )
{
	m_imp->m_clients.insert( &client ) ;
	m_imp->m_client_adds++ ;
}

void GNet::Monitor::remove( const Client & client )
{
	m_imp->m_client_removes++ ;
	m_imp->m_clients.erase( &client ) ;
}

void GNet::Monitor::add( const ServerPeer & peer )
{
	m_imp->m_server_peer_adds++ ;
	m_imp->m_server_peers.insert( & peer ) ;
}

void GNet::Monitor::remove( const ServerPeer & peer )
{
	m_imp->m_server_peer_removes++ ;
	m_imp->m_server_peers.erase( & peer ) ;
}

void GNet::Monitor::report( std::ostream & stream )
{
	stream << "clients created: " << m_imp->m_client_adds << std::endl ;
	stream << "client destroyed: " << m_imp->m_client_removes << std::endl ;
	{
		for( MonitorImp::Clients::const_iterator p = m_imp->m_clients.begin() ;
		 p != m_imp->m_clients.end() ; ++p )
		{
			stream
				<< "  client " << (const void *)(*p) << ": "
				<< (*p)->localAddress().second.displayString() << " -> "
				<< (*p)->peerAddress().second.displayString() << std::endl ;
		}
	}

	stream << "servers created: " << m_imp->m_server_peer_adds << std::endl ;
	stream << "servers destroyed: " << m_imp->m_server_peer_removes << std::endl ;

	{
		for( MonitorImp::ServerPeers::const_iterator p = m_imp->m_server_peers.begin() ;
			p != m_imp->m_server_peers.end() ; ++p )
		{
			stream
				<< "  server " << (const void *)(*p) << ": "
				<< (*p)->localAddress().second.displayString() << " -> "
				<< (*p)->peerAddress().second.displayString() << std::endl ;
		}
	}
}

