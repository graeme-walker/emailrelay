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
// gadminserver.h
//

#ifndef G_SMTP_ADMIN_H
#define G_SMTP_ADMIN_H

#include "gdef.h"
#include "gsmtp.h"
#include "gserver.h"
#include "glinebuffer.h"
#include "gserverprotocol.h"
#include "gsmtpclient.h"
#include <string>
#include <sstream>

namespace GSmtp
{
	class AdminPeer ;
	class AdminServer ;
	class AdminClient ;
} ;

// Class: GSmtp::AdminClient
// Description: A private implementation class.
//
class GSmtp::AdminClient : public GSmtp:: Client
{
public:
	AdminClient( AdminPeer & admin_peer ) ;
} ;

// ===

// Class: GSmtp::AdminPeer
// Description: A derivation of ServerPeer for the administration interface.
// See also: AdminServer
//
class GSmtp::AdminPeer : public GNet::ServerPeer , public GSmtp:: Client::ClientCallback
{
public:
	AdminPeer( GNet::StreamSocket * , GNet::Address , AdminServer & server , const std::string & ) ;
		// Constructor.

private:
	AdminPeer( const AdminPeer & ) ;
	void operator=( const AdminPeer & ) ;
	virtual void onDelete() ; // from GNet::ServerPeer
	virtual void onData( const char * , size_t ) ; // from GNet::ServerPeer
	virtual void clientDone( std::string ) ; // from Client::ClientCallback
	bool processLine( const std::string & line ) ;
	static bool is( const std::string & , const char * ) ;
	void flush( const std::string & ) ;
	void help() ;
	void send( std::string ) ;

private:
	GNet::LineBuffer m_buffer ;
	static std::string crlf() ;
	AdminServer & m_server ;
	std::auto_ptr<GSmtp::AdminClient> m_client ;
	std::string m_server_address ;
} ;

// Class: GSmtp::AdminServer
// Description: A server class which implements the emailrelay administration interface.
//
class GSmtp::AdminServer : public GNet::Server
{
public:
	AdminServer( unsigned int port , bool allow_remote , const std::string & server_address ) ;
		// Constructor.

private:
	virtual GNet::ServerPeer * newPeer( GNet::StreamSocket * , GNet::Address ) ;
	AdminServer( const AdminServer & ) ;
	void operator=( const AdminServer & ) ;

private:
	bool m_allow_remote ;
	std::string m_server_address ;
} ;

#endif
