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
// gsmtpclient.cpp
//

#include "gdef.h"
#include "gnet.h"
#include "gsmtp.h"
#include "glocal.h"
#include "gfile.h"
#include "gstr.h"
#include "gmemory.h"
#include "gsmtpclient.h"
#include "gresolve.h"
#include "glog.h"

//static
std::string GSmtp::Client::crlf()
{
	return std::string("\015\012") ;
}

GSmtp::Client::Client( GSmtp::MessageStore & store , bool quit_on_disconnect ) :
	GNet::Client(false,quit_on_disconnect) ,
	m_callback(NULL) ,
	m_store(store) ,
	m_buffer(crlf()) ,
	m_protocol(*this,GNet::Local::fqdn()) ,
	m_socket(NULL)
{
}

GSmtp::Client::Client( GSmtp::MessageStore & store , ClientCallback & callback , bool quit_on_disconnect ) :
	GNet::Client(false,quit_on_disconnect) ,
	m_callback(&callback) ,
	m_store(store) ,
	m_buffer(crlf()) ,
	m_protocol(*this,GNet::Local::fqdn()) ,
	m_socket(NULL)
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
	if( m_store.empty() )
		return "no messages to send" ;

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
	else if( rc < line.length() )
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
	m_socket = &socket ;
	m_iter = m_store.iterator() ;
	if( !sendNext() )
		finish() ;
}

void GSmtp::Client::finish()
{
	if( m_callback != NULL )
	{
		m_callback->onCompletion(std::string()) ;
		m_callback = NULL ;
	}

	disconnect() ; // GNet::Client::disconnect()
}

bool GSmtp::Client::sendNext()
{
	m_message <<= 0 ;

	{
		std::auto_ptr<StoredMessage> message( m_iter.next() ) ;
		if( message.get() == NULL )
		{
			G_LOG( "GSmtp::Client: no more messages to send" ) ;
			GNet::Socket * s = m_socket ;
			m_socket = NULL ;
			s->close() ;
			return false ;
		}
		m_message = message ;
	}

	m_protocol.start( m_message->from() , m_message->to() , m_message->eightBit() ,
		m_message->extractContentStream() , *this ) ;
	return true ;
}

void GSmtp::Client::callback( bool ok )
{
	G_DEBUG( "GSmtp::Client::callback: " << ok ) ;
	if( m_message.get() != NULL )
	{
		if( ok )
			m_message->destroy() ;
		else
			m_message->fail("smtp protocol failure") ;
	}
	if( !sendNext() )
		finish() ;
}

void GSmtp::Client::onDisconnect()
{
	if( m_callback != NULL )
		m_callback->onCompletion( "connection to server lost" ) ;
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

void GSmtp::Client::onError( const std::string &error )
{
	if( m_callback != NULL )
		m_callback->onCompletion( std::string("error on connection to server: ") + error ) ;
	G_WARNING( "GSmtp::Client: error: \"" << error << "\"" ) ;
}

void GSmtp::Client::onWriteable()
{
	if( protocolSend(m_pending) )
	{
		m_protocol.sendComplete() ;
	}
}

// ===

void GSmtp::Client::ClientCallback::onCompletion( std::string )
{
	// empty
}

