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
// gclientprotocol.h
//

#ifndef G_SMTP_CLIENT_PROTOCOL_H
#define G_SMTP_CLIENT_PROTOCOL_H

#include "gdef.h"
#include "gnet.h"
#include "gsmtp.h"
#include "gmessagestore.h"
#include "gstrings.h"
#include "gexception.h"
#include <memory>
#include <iostream>

namespace GSmtp
{
	class ClientProtocol ;
	class ClientProtocolReply ;
} ;

// Class: GSmtp::ClientProtocolReply
// Description: A private implementation class used
// by ClientProtocol.
//
class GSmtp::ClientProtocolReply
{
public:
	enum Type
	{
		PositivePreliminary = 1 ,
		PositiveCompletion = 2 ,
		PositiveIntermediate = 3 ,
		TransientNegative = 4 ,
		PermanentNegative = 5
	} ;
	enum SubType
	{
		Syntax = 0 ,
		Information = 1 ,
		Connections = 2 ,
		MailSystem = 3 ,
		Invalid_SubType = 4
	} ;
	enum Value
	{
		Invalid = 0 ,
		ServiceReady_220 = 220 ,
		SyntaxError_500 = 500 ,
		BadSequence_503 = 503 ,
		NotImplemented_502 = 502 ,
		OkForData_354 = 354 ,
		Ok_250 = 250
	} ;
	explicit ClientProtocolReply( const std::string & line = std::string() ) ;
	bool incomplete() const ;
	bool add( const ClientProtocolReply & other ) ;
	bool validFormat() const ;
	bool positive() const ; // <400
	bool is( Value v ) const ;
	unsigned int value() const ;
	std::string text() const ;
	Type type() const ;
	SubType subType() const ;
	bool textContains( std::string s ) const ;
private:
	bool m_complete ;
	bool m_valid ;
	unsigned int m_value ;
	std::string m_text ;
private:
	static bool is_digit( char ) ;
} ;

// Class: GSmtp::ClientProtocol
// Description: Implements the client-side SMTP protocol.
//
class GSmtp::ClientProtocol
{
public:
	G_EXCEPTION( NotReady , "not ready" ) ;
	G_EXCEPTION( NoRecipients , "no recipients" ) ;
	G_EXCEPTION( NarrowPipe , "cannot send an 8-bit message to a 7-bit server" ) ;
	typedef ClientProtocolReply Reply ;

	class Sender // An interface used by ClientProtocol to send protocol messages.
	{
		public: virtual bool protocolSend( const std::string & ) = 0 ;
			// Called by the Protocol class to send
			// network data to the peer.
			//
			// Returns false if not all of the string
			// was sent, either due to flow control
			// or disconnection. After false os returned
			// the user should call sendComplete() once
			// the full string has been sent.

		private: void operator=( const Sender & ) ;
		public: virtual ~Sender() {}
	} ;

	class Callback // A callback interface used by ClientProtocol.
	{
		public: virtual void callback( bool ok ) = 0 ;
			// Called once the protocol has finished with
			// a given message. See ClientProtocol::start().

		private: void operator=( const Callback & ) ;
		public: virtual ~Callback() {}
	} ;

	ClientProtocol( Sender & sender , const std::string & thishost ) ;
		// Constructor. The sender reference is kept.
		// The Sender interface is used to send
		// protocol messages to the peer.

	void start( const std::string & from , const G::Strings & to , bool eight_bit ,
		std::auto_ptr<std::istream> content , Callback & callback ) ;
			// Starts transmission of the given message.
			//
			// The 'callback' parameter is used to
			// signal that the message has been
			// processed.

	void sendComplete() ;
		// Called when a blocked connection becomes unblocked.
		// See ClientProtocol::Sender::protocolSend().

	void apply( const std::string & rx ) ;
		// Called on receipt of a line of text from the server.

	bool done() const ;
		// Returns true if the protocol is in the end state.

private:
	bool send( const std::string & , bool eot = false , bool log = true ) ;
	bool sendLine() ;
	void sendMail() ;
	bool endOfContent() const ;
	static std::string crlf() ;
	void applyEvent( const Reply & event ) ;
	static bool parseReply( Reply & , const std::string & , std::string & ) ;

private:
	enum State { sStart , sSentEhlo , sSentHelo , sSentMail ,
		sSentRcpt , sSentData , sData , sDone , sEnd , sReset } ;
	Sender & m_sender ;
	std::string m_thishost ;
	State m_state ;
	std::string m_from ;
	G::Strings m_to ;
	Callback * m_callback ;
	std::auto_ptr<std::istream> m_content ;
	bool m_server_has_8bitmime ;
	bool m_said_hello ;
	bool m_message_is_8bit ;
	Reply m_reply ;
} ;

#endif
