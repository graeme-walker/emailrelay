//
// Copyright (C) 2001-2002 Graeme Walker <graeme_walker@users.sourceforge.net>
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
// gsmtpclient.cpp
//

#include "gdef.h"
#include "gnet.h"
#include "gsmtp.h"
#include "glocal.h"
#include "gfile.h"
#include "gstr.h"
#include "gmemory.h"
#include "gtimer.h"
#include "gsmtpclient.h"
#include "gresolve.h"
#include "gassert.h"
#include "glog.h"

namespace
{
	const bool must_authenticate = true ;
} ;

unsigned int GSmtp::Client::m_response_timeout = 0U ;
unsigned int GSmtp::Client::m_connection_timeout = 0U ;

//static
std::string GSmtp::Client::crlf()
{
	return std::string("\015\012") ;
}

GSmtp::Client::Client( MessageStore & store , ClientCallback & callback ,
	bool quit_on_disconnect ) :
		GNet::Client(false,quit_on_disconnect) ,
		m_store(&store) ,
		m_buffer(crlf()) ,
		m_protocol(*this,GNet::Local::fqdn(),m_response_timeout,must_authenticate) ,
		m_socket(NULL) ,
		m_callback(&callback) ,
		m_connect_timer(*this) ,
		m_message_index(0U)
{
}

GSmtp::Client::Client( std::auto_ptr<StoredMessage> message , ClientCallback & callback ) :
	GNet::Client(false,false) ,
	m_store(NULL) ,
	m_message(message) ,
	m_buffer(crlf()) ,
	m_protocol(*this,GNet::Local::fqdn(),m_response_timeout,must_authenticate) ,
	m_socket(NULL) ,
	m_callback(&callback) ,
	m_connect_timer(*this) ,
	m_message_index(0U)
{
}

std::string GSmtp::Client::init( const std::string & s )
{
	size_t pos = s.find(':') ;
	if( pos == std::string::npos )
		return "invalid address string: no colon (<host/ip>:<service/port>)" ;

	return init( s.substr(0U,pos) , s.substr(pos+1U) ) ;
}

std::string GSmtp::Client::init( const std::string & host , const std::string & service )
{
	m_message_index = 0U ;
	m_host = host ;

	if( m_store != NULL && m_store->empty() )
		return "no messages to send" ;

	doStatusChange( "connecting" , host ) ;

	if( m_connection_timeout != 0U )
		m_connect_timer.startTimer( m_connection_timeout ) ;

	std::string error ;
	bool rc = connect( host , service , &error ) ;
	return rc ? std::string() : error ;
}

bool GSmtp::Client::busy() const
{
	return connected() ; // GNet::Client::connected()
}

bool GSmtp::Client::protocolSend( const std::string & line )
{
	ssize_t rc = socket().write( line.data() , line.length() ) ;
	if( rc < 0 )
	{
		m_pending = line ;
		if( socket().eWouldBlock() )
			blocked() ;
		return false ;
	}
	else if( rc < 0 || static_cast<size_t>(rc) < line.length() )
	{
		m_pending = line.substr(rc) ;
		blocked() ; // GNet::Client::blocked() => addWriteHandler()
		return false ;
	}
	else
	{
		return true ;
	}
}

void GSmtp::Client::onConnect( GNet::Socket & socket )
{
	m_connect_timer.cancelTimer() ;

	doStatusChange( "connected" ,
		socket.getPeerAddress().second.displayString() ) ;

	m_socket = &socket ;
	if( m_store != NULL )
	{
		m_iter = m_store->iterator() ;
		if( !sendNext() )
			finish() ;
	}
	else
	{
		G_ASSERT( m_message.get() != NULL ) ;
		start( *m_message.get() ) ;
	}
}

bool GSmtp::Client::sendNext()
{
	m_message <<= 0 ;

	{
		std::auto_ptr<StoredMessage> message( m_iter.next() ) ;
		if( message.get() == NULL )
		{
			G_LOG_S( "GSmtp::Client: no more messages to send" ) ;
			GNet::Socket * s = m_socket ;
			m_socket = NULL ;
			s->close() ;
			return false ;
		}
		m_message = message ;
	}

	start( *m_message.get() ) ;
	return true ;
}

void GSmtp::Client::start( StoredMessage & message )
{
	m_message_index++ ;
	doStatusChange( "sending" , G::Str::fromUInt(m_message_index) ) ; // or G::Str::join(message.to(),",") or whatever

	std::string server_name = peerName() ; // (from GNet::Client)
	if( server_name.empty() )
		server_name = m_host ;

	m_protocol.start( message.from() , message.to() , message.eightBit() ,
		message.authentication() , server_name , message.extractContentStream() , *this ) ;
}

void GSmtp::Client::protocolDone( bool ok , bool abort , const std::string & reason )
{
	G_DEBUG( "GSmtp::Client::protocolDone: " << ok << ": " << reason ) ;

	std::string error_message ;
	if( !ok )
		error_message = std::string("smtp client protocol failure: ") + reason ;

	if( m_message.get() != NULL )
	{
		if( ok )
			m_message->destroy() ;
		else
			m_message->fail( error_message ) ;
	}

	if( m_store == NULL || abort || !sendNext() )
	{
		finish( error_message ) ;
	}
}

void GSmtp::Client::onDisconnect()
{
	doCallback( "connection to server lost" ) ;
	m_socket = NULL ;
}

GNet::Socket & GSmtp::Client::socket()
{
	if( m_socket == NULL )
		throw NotConnected() ;
	return * m_socket ;
}

void GSmtp::Client::onData( const char * data , size_t size )
{
	m_buffer.add( std::string(data,size) ) ;
	while( m_buffer.more() )
	{
		m_protocol.apply( m_buffer.line() ) ;
		if( m_protocol.done() )
			finish() ;
	}
}

void GSmtp::Client::onError( const std::string & error )
{
	G_WARNING( "GSmtp::Client: smtp client error: \"" << error << "\"" ) ;
	doCallback( std::string("error on connection to server: ") + error ) ;
}

void GSmtp::Client::finish( const std::string & reason )
{
	doCallback( reason ) ;
	disconnect() ; // GNet::Client::disconnect()
}

void GSmtp::Client::doCallback( const std::string & reason )
{
	if( m_callback != NULL )
	{
		m_callback->clientStatusChange( "done" , "" ) ;
		m_callback->clientDone( reason ) ;
		m_callback = NULL ;
	}
}

void GSmtp::Client::doStatusChange( const std::string & s1 , const std::string & s2 )
{
	if( m_callback != NULL )
		m_callback->clientStatusChange( s1 , s2 ) ;
}

void GSmtp::Client::onWriteable()
{
	if( protocolSend(m_pending) )
	{
		m_protocol.sendDone() ;
	}
}

unsigned int GSmtp::Client::responseTimeout( unsigned int new_timeout )
{
	unsigned int previous = m_response_timeout ;
	m_response_timeout = new_timeout ;
	return previous ;
}

unsigned int GSmtp::Client::connectionTimeout( unsigned int new_timeout )
{
	unsigned int previous = m_connection_timeout ;
	m_connection_timeout = new_timeout ;
	return previous ;
}

void GSmtp::Client::onTimeout( GNet::Timer & )
{
	//G_ASSERT( &timer == &m_connect_timer ) ;
	doCallback( "timeout while connecting to server" ) ;
}

// ===

GSmtp::Client::ClientCallback::~ClientCallback()
{
}

void GSmtp::Client::ClientCallback::clientStatusChange( const std::string & s1 ,
	const std::string & s2 )
{
	(void) s1.length() ; (void) s2.length() ; // pacify the compiler
	G_DEBUG( "GSmtp::Client::ClientCallback::clientStatusChange: \"" << s1 << "\": \"" << s2 << "\"" ) ;
}

