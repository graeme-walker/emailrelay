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
// gadminserver.cpp
//

#include "gdef.h"
#include "gnet.h"
#include "gsmtp.h"
#include "gadminserver.h"
#include "gmonitor.h"
#include "gstr.h"
#include "gmemory.h"

GSmtp::AdminClient::AdminClient( AdminPeer & admin_peer ) :
	Client(MessageStore::instance(),admin_peer,false)
{
}

// ===

GSmtp::AdminPeer::AdminPeer( GNet::StreamSocket * s , GNet::Address a , AdminServer & server ,
	const std::string & server_address ) :
		GNet::ServerPeer( s , a ) ,
		m_server(server) ,
		m_buffer(crlf()) ,
		m_server_address(server_address)
{
	// dont prompt() here -- it confuses the poke program
}

void GSmtp::AdminPeer::clientDone( std::string s )
{
	if( s.empty() )
		send( "OK" ) ;
	else
		send( std::string("error: ") + s ) ;

	prompt() ;
}

void GSmtp::AdminPeer::onDelete()
{
}

void GSmtp::AdminPeer::onData( const char * data , size_t n )
{
	m_buffer.add( std::string(data,n) ) ;
	while( m_buffer.more() )
	{
		if( ! processLine( m_buffer.line() ) )
			return ;
	}
}

bool GSmtp::AdminPeer::processLine( const std::string & line )
{
	if( is(line,"FLUSH") )
	{
		flush( m_server_address ) ;
	}
	else if( is(line,"HELP") )
	{
		help() ;
		prompt() ;
	}
	else if( is(line,"INFO") )
	{
		info() ;
		prompt() ;
	}
	else if( is(line,"QUIT") )
	{
		doDelete() ;
		return false ;
	}
	else if( line.find_first_not_of(" \r\n\t") != std::string::npos )
	{
		send( "error: unrecognised command" ) ;
		prompt() ;
	}
	else
	{
		prompt() ;
	}
	return true ;
}

//static
std::string GSmtp::AdminPeer::crlf()
{
	return "\015\012" ;
}

//static
bool GSmtp::AdminPeer::is( const std::string & line_in , const char * key )
{
	std::string line( line_in ) ;
	G::Str::trim( line , " \t" ) ;
	G::Str::toUpper( line ) ;
	return line.find(key) == 0U ;
}

void GSmtp::AdminPeer::help()
{
	send( "commands: FLUSH, HELP, INFO, QUIT" ) ;
}

void GSmtp::AdminPeer::flush( const std::string & address )
{
	G_DEBUG( "GSmtp::AdminPeer: flush: \"" << address << "\"" ) ;

	if( m_client.get() != NULL && m_client->busy() )
	{
		send( "error: still working" ) ;
	}
	else
	{
		m_client <<= new AdminClient( *this ) ;
		std::string rc = m_client->init( address ) ;
		if( rc.length() != 0U )
		{
			send( std::string("error: ") + rc ) ;
		}
	}
}

void GSmtp::AdminPeer::prompt()
{
	std::string p( "E-MailRelay> " ) ;
	ssize_t rc = socket().write( p.data() , p.length() ) ;
	if( rc < p.length() )
		doDelete() ; // onDelete() and "delete this"
}

void GSmtp::AdminPeer::send( std::string line )
{
	line.append( crlf() ) ;
	ssize_t rc = socket().write( line.data() , line.length() ) ;
	if( rc < line.length() )
		doDelete() ; // onDelete() and "delete this"
}

void GSmtp::AdminPeer::info()
{
	std::stringstream ss ;
	if( GNet::Monitor::instance() )
	{
		GNet::Monitor::instance()->report( ss , "" , crlf() ) ;
		send( ss.str() ) ;
	}
	else
	{
		send( "no info" ) ;
	}
}

// ===

GSmtp::AdminServer::AdminServer( unsigned int port , bool allow_remote , const std::string & address ) :
	GNet::Server( port ) ,
	m_allow_remote( allow_remote ) ,
	m_server_address( address )
{
	G_DEBUG( "GSmtp::AdminServer: administrative interface listening on port " << port ) ;
}

GNet::ServerPeer * GSmtp::AdminServer::newPeer( GNet::StreamSocket * s , GNet::Address a )
{
	return new AdminPeer( s , a , *this , m_server_address ) ;
}

