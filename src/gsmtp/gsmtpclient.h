//
// Copyright (C) 2001-2003 Graeme Walker <graeme_walker@users.sourceforge.net>
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
// gsmtpclient.h
//

#ifndef G_SMTP_CLIENT_H
#define G_SMTP_CLIENT_H

#include "gdef.h"
#include "gnet.h"
#include "gsmtp.h"
#include "gsecrets.h"
#include "glinebuffer.h"
#include "gclient.h"
#include "gclientprotocol.h"
#include "gmessagestore.h"
#include "gstoredmessage.h"
#include "gsocket.h"
#include "gtimer.h"
#include "gstrings.h"
#include "gexception.h"
#include <memory>
#include <iostream>

namespace GSmtp
{
	class Client ;
	class ClientProtocol ;
}

// Class: GSmtp::Client
// Description: A class which acts as an SMTP client, extracting
// messages from a message store and forwarding them to
// a remote SMTP server.
//
class GSmtp::Client : private GNet::Client , private GNet::TimeoutHandler ,
	private GSmtp:: ClientProtocol::Sender ,
	private GSmtp:: ClientProtocol::Callback
{
public:
	G_EXCEPTION( NotConnected , "not connected" ) ;
	class ClientCallback // A callback interface used by GSmtp::Client.
	{
		public: virtual void clientDone( std::string ) = 0 ;
		public: virtual void clientEvent( const std::string & , const std::string & ) ;
		public: virtual ~ClientCallback() ;
		private: void operator=( const ClientCallback & ) ; // not implemented
	} ;

	Client( MessageStore & store , ClientCallback & callback , bool quit_on_disconnect ) ;
			// Constructor. The message-store and callback
			// references are kept.
			//
			// The callback is used to signal that
			// all message processing has finished
			// or that the server connection has
			// been lost.

	Client( std::auto_ptr<StoredMessage> message , ClientCallback & callback ) ;
			// Constructor for sending a single message.
			//
			// The callback is used to signal that
			// all message processing has finished
			// or that the server connection has
			// been lost.
			//
			// With this constructor the message is fail()ed
			// if the connection to the downstream server
			// cannot be made.

	std::string init( const std::string & server_address_string ) ;
		// Starts the sending process. Messages
		// are extracted from the message store
		// (as passed in the ctor) and forwarded
		// on to the specified server.
		//
		// To be called once (only) after construction.
		//
		// Returns an error string if there are no messages
		// to be sent, or if the network connection
		// cannot be initiated. Returns the empty
		// string on success.

	bool busy() const ;
		// Returns true after construction and while
		// message processing is going on. Returns
		// false once the clientDone() callback
		// has been fired.

	static unsigned int responseTimeout( unsigned int new_timeout ) ;
		// Sets the response timeout value. Returns the
		// previous value.

	static unsigned int connectionTimeout( unsigned int new_timeout ) ;
		// Sets the connection timeout value. Returns the
		// previous value.


private:
	virtual void onConnect( GNet::Socket & socket ) ; // GNet::Client
	virtual void onDisconnect() ; // GNet::Client
	virtual void onData( const char * data , size_t size ) ; // GNet::Client
	virtual void onWriteable() ; // GNet::Client
	virtual void onError( const std::string & error ) ; // GNet::Client
	virtual bool protocolSend( const std::string & ) ; // ClientProtocol::Sender
	virtual void protocolDone( bool , bool , const std::string & ) ; // ClientProtocol::Callback
	virtual void onTimeout( GNet::Timer & ) ; // GNet::TimeoutHandler
	std::string init( const std::string & , const std::string & ) ;
	GNet::Socket & socket() ;
	static std::string crlf() ;
	bool sendNext() ;
	void start( StoredMessage & ) ;
	void doCallback( const std::string & ) ;
	void raiseEvent( const std::string & , const std::string & ) ;
	void finish( const std::string & reason = std::string() , bool do_disconnect = true ) ;
	void messageFail( const std::string & reason ) ;
	void messageDestroy() ;

private:
	MessageStore * m_store ;
	std::auto_ptr<StoredMessage> m_message ;
	MessageStore::Iterator m_iter ;
	GNet::LineBuffer m_buffer ;
	ClientProtocol m_protocol ;
	GNet::Socket * m_socket ;
	std::string m_pending ;
	ClientCallback * m_callback ;
	std::string m_host ;
	GNet::Timer m_connect_timer ;
	unsigned int m_message_index ;
	bool m_force_message_fail ;
	static unsigned int m_response_timeout ;
	static unsigned int m_connection_timeout ;
} ;

#endif