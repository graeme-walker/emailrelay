//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gpopserver.h
///

#ifndef G_POP_SERVER_H
#define G_POP_SERVER_H

#include "gdef.h"
#include "gmultiserver.h"
#include "glinebuffer.h"
#include "gpopserverprotocol.h"
#include "gsecrets.h"
#include "gexception.h"
#include "gstringview.h"
#include "gstringarray.h"
#include <string>
#include <sstream>
#include <memory>
#include <list>

namespace GPop
{
	class Server ;
	class ServerPeer ;
}

//| \class GPop::ServerPeer
/// Represents a connection from a POP client. Instances are created
/// on the heap by GPop::Server.
/// \see GPop::Server
///
class GPop::ServerPeer : public GNet::ServerPeer , private ServerProtocol::Sender , private ServerProtocol::Security
{
public:
	G_EXCEPTION( SendError , tx("network send error") )

	ServerPeer( GNet::EventStateUnbound , GNet::ServerPeerInfo && , Store & ,
		const GAuth::SaslServerSecrets & , const std::string & sasl_server_config ,
		std::unique_ptr<ServerProtocol::Text> ptext , const ServerProtocol::Config & ) ;
			///< Constructor.

private: // overrides
	bool protocolSend( std::string_view , std::size_t ) override ; // GPop::ServerProtocol::Sender
	void onDelete( const std::string & ) override ; // GNet::ServerPeer
	bool onReceive( const char * , std::size_t , std::size_t , std::size_t , char ) override ; // GNet::ServerPeer
	void onSecure( const std::string & , const std::string & , const std::string & ) override ; // GNet::SocketProtocolSink
	void onSendComplete() override ; // GNet::ServerPeer
	bool securityEnabled() const override ; // GPop::ServerProtocol::Security
	void securityStart() override ; // GPop::ServerProtocol::Security

public:
	~ServerPeer() override = default ;
	ServerPeer( const ServerPeer & ) = delete ;
	ServerPeer( ServerPeer && ) = delete ;
	ServerPeer & operator=( const ServerPeer & ) = delete ;
	ServerPeer & operator=( ServerPeer && ) = delete ;

private:
	void processLine( const std::string & line ) ;

private:
	std::unique_ptr<ServerProtocol::Text> m_ptext ; // order dependency
	ServerProtocol m_protocol ; // order dependency -- last
} ;

//| \class GPop::Server
/// A POP server class.
///
class GPop::Server : public GNet::MultiServer
{
public:
	G_EXCEPTION( Overflow , tx("too many interface addresses") )
	struct Config /// A structure containing GPop::Server configuration parameters.
	{
		bool allow_remote {false} ;
		unsigned int port {110} ;
		G::StringArray addresses ;
		GNet::ServerPeer::Config net_server_peer_config ;
		GNet::Server::Config net_server_config ;
		ServerProtocol::Config protocol_config ;
		std::string sasl_server_config ;

		Config & set_allow_remote( bool = true ) noexcept ;
		Config & set_port( unsigned int ) noexcept ;
		Config & set_addresses( const G::StringArray & ) ;
		Config & set_net_server_peer_config( const GNet::ServerPeer::Config & ) ;
		Config & set_net_server_config( const GNet::Server::Config & ) ;
		Config & set_protocol_config( const ServerProtocol::Config & ) ;
		Config & set_sasl_server_config( const std::string & ) ;
	} ;

	Server( GNet::EventState , Store & store , const GAuth::SaslServerSecrets & , const Config & ) ;
		///< Constructor. The 'secrets' reference is kept.

	~Server() override ;
		///< Destructor.

	void report( const std::string & group = {} ) const ;
		///< Generates helpful diagnostics after construction.

private: // overrides
	std::unique_ptr<GNet::ServerPeer> newPeer( GNet::EventStateUnbound , GNet::ServerPeerInfo && , GNet::MultiServer::ServerInfo ) override ;

public:
	Server( const Server & ) = delete ;
	Server( Server && ) = delete ;
	Server & operator=( const Server & ) = delete ;
	Server & operator=( Server && ) = delete ;

private:
	std::unique_ptr<ServerProtocol::Text> newProtocolText( const GNet::Address & ) const ;

private:
	Config m_config ;
	Store & m_store ;
	const GAuth::SaslServerSecrets & m_secrets ;
} ;

inline GPop::Server::Config & GPop::Server::Config::set_allow_remote( bool b ) noexcept { allow_remote = b ; return *this ; }
inline GPop::Server::Config & GPop::Server::Config::set_port( unsigned int p ) noexcept { port = p ; return *this ; }
inline GPop::Server::Config & GPop::Server::Config::set_addresses( const G::StringArray & a ) { addresses = a ; return *this ; }
inline GPop::Server::Config & GPop::Server::Config::set_net_server_peer_config( const GNet::ServerPeer::Config & c ) { net_server_peer_config = c ; return *this ; }
inline GPop::Server::Config & GPop::Server::Config::set_net_server_config( const GNet::Server::Config & c ) { net_server_config = c ; return *this ; }
inline GPop::Server::Config & GPop::Server::Config::set_protocol_config( const ServerProtocol::Config & c ) { protocol_config = c ; return *this ; }
inline GPop::Server::Config & GPop::Server::Config::set_sasl_server_config( const std::string & s ) { sasl_server_config = s ; return *this ; }

#endif
