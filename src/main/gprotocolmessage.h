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
// gprotocolmessage.h
//

#ifndef G_SMTP_PROTOCOL_MESSAGE_H
#define G_SMTP_PROTOCOL_MESSAGE_H

#include "gdef.h"
#include "gsmtp.h"
#include "gstrings.h"
#include "gverifier.h"
#include <string>

namespace GSmtp
{
	class ProtocolMessage ;
} ;

// Class: GSmtp::ProtocolMessage
// Description: An interface used by the ServerProtocol
// class to assemble and process an incoming message.
// It implements the three 'buffers' mentioned in
// RFC2821 (esp. section 4.1.1).
//
// This interface serves to decouple the ServerProtocol class
// from the MessageStore (or whatever else is downstream).
//
class GSmtp::ProtocolMessage
{
public:
	class Callback // A callback interface used by ProtocolMessage::process().
	{
		public: virtual ~Callback() ;
		public: virtual void processDone( bool success , unsigned long id , const std::string & reason ) = 0 ;
		private: void operator=( const Callback & ) ; // not implemented
	} ;

	virtual ~ProtocolMessage() ;
		// Destructor.

	virtual void clear() = 0 ;
		// Clears the message state and terminates
		// any asynchronous message processing.

	virtual bool setFrom( const std::string & from_user ) = 0 ;
		// Sets the message envelope 'from'.
		// Returns false if an invalid user.

	virtual bool addTo( const std::string & to_user , Verifier::Status to_status ) = 0 ;
		// Adds an envelope 'to'.
		//
		// The 'to_status' parameter comes from
		// GSmtp::Verifier.verify().
		//
		// Returns false if an invalid user.
		// Precondition: setFrom() called
		// since clear() or process().

	virtual void addReceived( const std::string & ) = 0 ;
		// Adds a 'received' line to the
		// start of the content.
		// Precondition: at least one
		// successful addTo() call

	virtual void addText( const std::string & ) = 0 ;
		// Adds text.
		// Precondition: at least one
		// successful addTo() call

	virtual void process( Callback & callback ) = 0 ;
		// Starts asynchronous processing of the
		// message. Once processing is complete the
		// message state is cleared and the callback
		// is triggered. The callback may be called
		// before process() returns.

private:
	void operator=( const ProtocolMessage & ) ; // not implemented
} ;

#endif

