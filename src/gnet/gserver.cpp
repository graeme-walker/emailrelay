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
// gserver.cpp
//

#include "gdef.h"
#include "gnet.h"
#include "gserver.h"
#include "gmonitor.h"
#include "gdebug.h"
#include "gassert.h"

GNet::ServerPeer::ServerPeer( StreamSocket * s , Address a )  :
	m_socket(s) ,
	m_address(a) ,
	m_ref_count(1U)
{
	G_ASSERT( m_socket != NULL ) ;
	G_DEBUG( "GNet::ServerPeer::ctor: fd " << m_socket->asString() << ": " << m_address.displayString() ) ;
	if( Monitor::instance() ) Monitor::instance()->add(*this) ;
	m_socket->addReadHandler( *this ) ;
}

GNet::ServerPeer::~ServerPeer()
{
	G_DEBUG( "GNet::ServerPeer::dtor: fd " << m_socket->asString() ) ;
	if( Monitor::instance() ) Monitor::instance()->remove(*this) ;
	m_socket->dropReadHandler() ;
	delete m_socket ;
	m_socket = NULL ;
}

std::string GNet::ServerPeer::asString() const
{
	return m_socket->asString() ; // ie. the fd
}

GNet::StreamSocket & GNet::ServerPeer::socket()
{
	G_ASSERT( m_socket != NULL ) ;
	return *m_socket ;
}

void GNet::ServerPeer::readEvent()
{
	G_DEBUG( "GNet::ServerPeer::readEvent: " << (void*)this ) ;

	char buffer[500] ;
	buffer[0] = '\0' ;
	size_t buffer_size = sizeof(buffer) ;
	ssize_t rc = m_socket->read( buffer , buffer_size ) ;

	if( rc <= 0 )
	{
		doDelete() ;
	}
	else
	{
		size_t n = rc ;
		onData( buffer , n ) ;
	}
}

void GNet::ServerPeer::doDelete()
{
	onDelete() ;
	delete this ;
}

void GNet::ServerPeer::up()
{
	m_ref_count++ ;
}

bool GNet::ServerPeer::down()
{
	m_ref_count-- ;
	return m_ref_count == 0U ;
}

std::pair<bool,GNet::Address> GNet::ServerPeer::localAddress() const
{
	G_ASSERT( m_socket != NULL ) ;
	return m_socket->getLocalAddress() ;
}

std::pair<bool,GNet::Address> GNet::ServerPeer::peerAddress() const
{
	G_ASSERT( m_socket != NULL ) ;
	return m_socket->getPeerAddress() ;
}

// ===

GNet::Server::Server( unsigned int listening_port ) :
	m_socket(NULL)
{
	try
	{
		init( listening_port ) ;
	}
	catch(...)
	{
		delete m_socket ;
		throw ;
	}
}

GNet::Server::Server() :
	m_socket(NULL)
{
}

void GNet::Server::init( unsigned int listening_port )
{
	m_socket = new StreamSocket ;
	G_DEBUG( "GNet::Server::init: " << (void*)this << ": listening on port " << listening_port ) ;
	Address local_address( listening_port ) ;
	if( ! m_socket->bind( local_address ) )
		throw CannotBind( local_address.displayString() ) ;
	if( ! m_socket->listen() )
		throw CannotListen() ;
	m_socket->addReadHandler( *this ) ;
}

GNet::Server::~Server()
{
	delete m_socket ;
}

void GNet::Server::readEvent()
{
	// read-event-on-listening-port => new connection to accept

	G_DEBUG( "GNet::Server::readEvent: " << (void*)this ) ;
	G_ASSERT( m_socket != NULL ) ;
	AcceptPair pair = m_socket->accept() ;
	if( pair.first.get() == NULL )
	{
		G_WARNING( "GNet::Server::readEvent: accept error" ) ;
	}
	else
	{
		G_DEBUG( "GNet::Server::readEvent: new connection from " << pair.second.displayString() ) ;
		ServerPeer * peer = newPeer(pair.first.release(),pair.second) ;
		if( peer != NULL )
			G_DEBUG( "GNet::Server::readEvent: new connection accepted onto fd " << peer->asString() ) ;
	}
}

void GNet::Server::writeEvent()
{
	G_DEBUG( "GNet::Server::writeEvent" ) ;
}

void GNet::Server::exceptionEvent()
{
	G_DEBUG( "GNet::Server::exceptionEvent" ) ;
}

