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
// gsmtpserver.h
//

#ifndef G_SMTP_SERVER_H
#define G_SMTP_SERVER_H

#include "gdef.h"
#include "gsmtp.h"
#include "gserver.h"
#include "glinebuffer.h"
#include "gserverprotocol.h"
#include <string>
#include <sstream>

namespace GSmtp
{
	class Server ;
	class ServerPeer ;
} ;

// Class: GSmtp::ServerPeer
// Description: Represents a connection from an SMTP client.
// Instances are created on the heap by Server (only).
// See also: Server
//
class GSmtp::ServerPeer : public GNet::ServerPeer , private GSmtp::ServerProtocol::Sender
{
public:
	ServerPeer( GNet::StreamSocket * , GNet::Address , Server & server , const std::string & ident ) ;
		// Constructor.

private:
	ServerPeer( const ServerPeer & ) ;
	void operator=( const ServerPeer & ) ;
	virtual void protocolSend( const std::string & line ) ; // from ServerProtocol::Sender
	virtual void protocolDone() ; // from ServerProtocol::Sender
	virtual void onDelete() ; // from GNet::ServerPeer
	virtual void onData( const char * , size_t ) ; // from GNet::ServerPeer
	bool processLine( const std::string & line ) ;
	std::string thishost() const ;

private:
	GNet::LineBuffer m_buffer ;
	static std::string crlf() ;
	ServerProtocol m_protocol ;
	Server & m_server ;
} ;

// Class: GSmtp::Server
// Description: An SMTP server class.
//
class GSmtp::Server : private GNet::Server
{
public:
	Server( unsigned int port , bool allow_remote , const std::string & ident ) ;
		// Constructor.

private:
	virtual GNet::ServerPeer * newPeer( GNet::StreamSocket * , GNet::Address ) ;
	Server( const Server & ) ;
	void operator=( const Server & ) ;

private:
	std::string m_ident ;
	bool m_allow_remote ;
} ;

#endif
