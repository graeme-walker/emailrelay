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
// gsmtpserver.cpp
//

#include "gdef.h"
#include "gsmtp.h"
#include "gsmtpserver.h"
#include "gprotocolmessagestore.h"
#include "gprotocolmessageforward.h"
#include "gmemory.h"
#include "glocal.h"
#include "glog.h"
#include "gdebug.h"
#include "gassert.h"
#include <string>

GSmtp::ServerPeer::ServerPeer( GNet::StreamSocket * socket , GNet::Address peer_address ,
	Server & server , std::auto_ptr<ProtocolMessage> pmessage , const std::string & ident ) :
		GNet::ServerPeer( socket , peer_address ) ,
		m_pmessage( pmessage ) ,
		m_protocol( *this , m_verifier , *m_pmessage.get() , thishost() , peer_address.displayString(false) ) ,
		m_buffer( crlf() ) ,
		m_server( server )
{
	G_LOG( "GSmtp::ServerPeer: new connection from " << peer_address.displayString() ) ;
	m_protocol.init( ident ) ;
}

std::string GSmtp::ServerPeer::thishost() const
{
	return GNet::Local::fqdn() ;
}

std::string GSmtp::ServerPeer::crlf()
{
	return std::string("\015\012") ;
}

void GSmtp::ServerPeer::onDelete()
{
	G_WARNING( "GSmtp::ServerPeer: peer disconnected" ) ;
}

void GSmtp::ServerPeer::onData( const char * p , size_t n )
{
	std::string s( p , n ) ;
	m_buffer.add( s ) ;
	while( m_buffer.more() )
	{
		bool this_deleted = processLine( m_buffer.line() ) ;
		if( this_deleted )
			break ;
	}
}

bool GSmtp::ServerPeer::processLine( const std::string & line )
{
	return m_protocol.apply( line ) ;
}

void GSmtp::ServerPeer::protocolSend( const std::string & line )
{
	if( line.length() == 0U )
		return ;

	ssize_t rc = socket().write( line.data() , line.length() ) ;
	if( rc == -1 && ! socket().eWouldBlock() )
	{
		doDelete() ; // onDelete() and "delete this"
	}
	else if( rc < line.length() )
	{
		G_ERROR( "GSmtp::ServerPeer::protocolSend: " <<
			"flow-control asserted: connection blocked" ) ;

		// an SMTP server only sends short status messages
		// back to the client so it is pretty wierd if the
		// client/network cannot cope -- so just drop the
		// connection
		//
		doDelete() ;
	}
}

void GSmtp::ServerPeer::protocolDone()
{
	G_LOG( "GSmtp::ServerPeer: disconnecting" ) ;
	doDelete() ; // onDelete() and "delete this"
}

// ===

GSmtp::Server::Server( unsigned int port , bool allow_remote , const std::string & ident ,
	const std::string & downstream_server ) :
		GNet::Server( port ) ,
		m_ident( ident ) ,
		m_allow_remote( allow_remote ) ,
		m_downstream_server(downstream_server)
{
	//G_LOG( "GSmtp::Server: listening on port " << port ) ;
}

GNet::ServerPeer * GSmtp::Server::newPeer( GNet::StreamSocket * socket , GNet::Address peer_address )
{
	std::auto_ptr<GNet::StreamSocket> socket_ptr(socket) ;

	if( ! m_allow_remote &&
		!peer_address.sameHost(GNet::Local::canonicalAddress()) &&
		!peer_address.sameHost(GNet::Local::localhostAddress()) )
	{
		G_WARNING( "GSmtp::Server: configured to reject non-local connection: "
			<< peer_address.displayString(false) << " is not one of "
			<< GNet::Local::canonicalAddress().displayString(false) << ","
			<< GNet::Local::localhostAddress().displayString(false) ) ;
		return NULL ;
	}

	const bool immediate = ! m_downstream_server.empty() ;

	std::auto_ptr<ProtocolMessage> pmessage(
		immediate ?
			static_cast<ProtocolMessage*>(new ProtocolMessageForward(m_downstream_server)) :
			static_cast<ProtocolMessage*>(new ProtocolMessageStore) ) ;

	return new ServerPeer( socket_ptr.release() , peer_address , *this , pmessage , m_ident ) ;
}

