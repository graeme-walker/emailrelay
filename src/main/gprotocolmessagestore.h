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
// gprotocolmessagestore.h
//

#ifndef G_SMTP_PROTOCOL_MESSAGE_STORE_H
#define G_SMTP_PROTOCOL_MESSAGE_STORE_H

#include "gdef.h"
#include "gsmtp.h"
#include "gprotocolmessage.h"
#include "gnewmessage.h"
#include <string>
#include <memory>

namespace GSmtp
{
	class ProtocolMessageStore ;
} ;

// Class: GSmtp::ProtocolMessageStore
// Description: A concrete implementation of the
// ProtocolMessage interface which stores incoming
// messages in the message store.
// See also: ProtocolMessageForward
//
class GSmtp::ProtocolMessageStore : public GSmtp:: ProtocolMessage
{
public:
	ProtocolMessageStore() ;
		// Constructor.

	virtual ~ProtocolMessageStore() ;
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

	virtual void process( ProtocolMessage::Callback & callback ) ;
		// See ProtocolMessage.

private:
	void operator=( const ProtocolMessageStore & ) ; // not implemented

private:
	std::auto_ptr<NewMessage> m_msg ;
} ;

#endif
