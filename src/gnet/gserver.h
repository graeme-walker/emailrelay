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
// gserver.h
//

#ifndef G_SERVER_H
#define G_SERVER_H

#include "gdef.h"
#include "gnet.h"
#include "gsocket.h"
#include "gconnection.h"
#include "gevent.h"
#include <utility>
#include <list>
#include <memory>
#include <string>

namespace GNet
{
	class Server ;
	class ServerPeer ;
}

// Class: GNet::Server
// Description: An application-level class for implementing a
// simple TCP server. The user must instantiate a separate
// event-source object (such as GNet::Select) for event handling.
// See also: GNet::ServerPeer
//
class GNet::Server : public GNet:: EventHandler
{
public:
	G_EXCEPTION( CannotBind , "cannot bind the listening port" ) ;
	G_EXCEPTION( CannotListen , "cannot listen" ) ;

	explicit Server( unsigned int listening_port ) ;
		// Constructor taking a port number. The server
		// listens on all local interfaces.

	explicit Server( const Address & listening_address ) ;
		// Constructor. The server listens only on the
		// specific (local) interface.

	Server() ;
		// Default constructor. Initialise with init().

	void init( unsigned int listening_port ) ;
		// Initilisation after default construction.

	void init( const Address & listening_address ) ;
		// Initilisation after default construction.

	virtual ~Server() ;
		// Destructor.

	std::pair<bool,Address> address() const ;
		// Returns the listening address. Pair.first
		// is false if not properly init()ialised.

protected:
	virtual ServerPeer * newPeer( StreamSocket * socket ,
		Address peer_address ) = 0 ;
			// A factory method which new()s a ServerPeer-derived
			// object. This method is called when a new connection
			// comes into this server. The new ServerPeer object
			// is used to represent the state of the client/server
			// connection.
			//
			// The 'socket' parameter points to a socket
			// object on the heap. Ownership is transferred.
			//
			// The implementation shoud pass the 'socket' and
			// 'peer_address' parameters through to the ServerPeer
			// base-class constructor.
			//
			// May return NULL.

private:
	Server( const Server & ) ; // not implemented
	void operator=( const Server & ) ; // not implemented
	virtual void readEvent() ; // from EventHandler
	virtual void writeEvent() ; // from EventHandler
	virtual void exceptionEvent() ; // from EventHandler

private:
	std::auto_ptr<StreamSocket> m_socket ;
} ;

// Class: GNet::ServerPeer
// Description: An abstract base class for the GNet::Server's
// connection to a remote client. Instances are created
// on the heap by the Server::newPeer() override, and they
// delete themselves when the connection is lost.
// See also: GNet::Server, GNet::EventHandler
//
class GNet::ServerPeer : public GNet:: EventHandler , public GNet:: Connection
{
public:
	ServerPeer( StreamSocket * , Address ) ;
		// Constructor. This constructor is
		// only used from within the
		// override of GServer::newPeer().

	void up() ;
		// Increases this object's internal reference
		// count. See also down(). Reference counting
		// is provided for the convenience of
		// reference-counting wrappers, but is
		// not used by this library.

	bool down() ;
		// Decreases this object's internal reference
		// count. Returns true if the reference
		// count is now zero.
		// Usage: if(down()) doDelete()

	void doDelete() ;
		// Does "onDelete(); delete this".

	std::string asString() const ;
		// Returns a string representation of the
		// socket descriptor. Typically used in
		// log message to destinguish one connection
		// from another.

	virtual std::pair<bool,Address> localAddress() const ;
		// Returns the local address.
		// Pair.first is false on error.

	virtual std::pair<bool,Address> peerAddress() const ;
		// Returns the peer address.

protected:
	virtual ~ServerPeer() ;
		// Destructor. Note that objects will delete
		// themselves when they detect that the
		// connection has been lost -- see doDelete().

	virtual void onDelete() = 0 ;
		// Called just before destruction. (Note
		// that the object typically deletes itself.)

	virtual void onData( const char * , size_t ) = 0 ;
		// Called on receipt of data.

	StreamSocket & socket() ;
		// Returns a reference to the client-server
		// connection socket.

private:
	virtual void readEvent() ; // from EventHandler
	virtual void exceptionEvent() ; // from EventHandler
	ServerPeer( const ServerPeer & ) ; // not implemented
	void operator=( const ServerPeer & ) ; // not implemented

private:
	unsigned int m_ref_count ;
	Address m_address ;
	StreamSocket * m_socket ;
} ;

#endif
