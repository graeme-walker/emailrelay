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
// gprotocolmessageforward.h
//

#ifndef G_SMTP_PROTOCOL_MESSAGE_FORWARD_H
#define G_SMTP_PROTOCOL_MESSAGE_FORWARD_H

#include "gdef.h"
#include "gsmtp.h"
#include "gprotocolmessage.h"
#include "gprotocolmessagestore.h"
#include "gsmtpclient.h"
#include "gnewmessage.h"
#include <string>
#include <memory>

namespace GSmtp
{
	class ProtocolMessageForward ;
} ;

// Class: GSmtp::ProtocolMessageForward
// Description: A concrete implementation of the ProtocolMessage
// interface which stores incoming messages in the message store
// and then immediately forwards them on to the downstream server.
//
// The implementation delegates to an instance of the ProtocolMessageStore
// class (ie. its sibling class) to do the storage, and to an instance
// of the Client class to do the forwarding.
//
// See also: ProtocolMessageStore
//
class GSmtp::ProtocolMessageForward : public GSmtp:: ProtocolMessage ,
	private GSmtp:: ProtocolMessage::Callback ,
	private GSmtp:: Client::ClientCallback
{
public:
	explicit ProtocolMessageForward( const std::string & server_address ) ;
		// Constructor.

	virtual ~ProtocolMessageForward() ;
		// Destructor.

	virtual void clear() ;
		// See ProtocolMessage.

	virtual bool setFrom( const std::string & from_user ) ;
		// See ProtocolMessage.

	virtual bool addTo( const std::string & to_user , Verifier::Status to_status ) ;
		// See ProtocolMessage.

	virtual void addReceived( const std::string & ) ;
		// See ProtocolMessage.

	virtual void addText( const std::string & ) ;
		// See ProtocolMessage.

	virtual void process( ProtocolMessage::Callback & callback , const std::string & auth_id ,
		const std::string & client_ip ) ;
			// See ProtocolMessage.

private:
	void operator=( const ProtocolMessageForward & ) ; // not implemented
	virtual void processDone( bool , unsigned long , const std::string & ) ; // from ProtocolMessage::Callback
	virtual void clientDone( std::string ) ; // from Client::ClientCallback
	bool forward( unsigned long , bool & , std::string * ) ;

private:
	ProtocolMessageStore m_pm ;
	std::string m_server ;
	ProtocolMessage::Callback * m_callback ;
	std::auto_ptr<Client> m_client ;
	unsigned long m_id ;
} ;

#endif
