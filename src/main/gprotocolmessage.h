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
#include <string>

namespace GSmtp
{
	class ProtocolMessage ;
	class ProtocolMessageImp ;
} ;

// Class: GSmtp::ProtocolMessage
// Description: An interface used by the ServerProtocol
// class to assemble and process an incoming message.
// It implements the three 'buffers' mentioned in
// RFC2821 (esp. section 4.1.1). Also does mail-address
// validation.
//
// This class serves to decouple the ServerProtocol class from
// the MessageStore (or whatever else is downstream).
//
class GSmtp::ProtocolMessage
{
public:
	ProtocolMessage() ;
		// Default constructor.

	~ProtocolMessage() ;
		// Destructor.

	static std::pair<bool,std::string> verify( const std::string & ) ;
		// Checks an address returning
		// <is-local>|<local-full-name>.
		//
		// (If syntactically local then 'first' is
		// returned true. If local and valid then
		// 'second' is set to the full description.
		// If syntactically remote, then 'first'
		// is returned false and 'second' is empty.)

	void clear() ;
		// Clears the message state.

	bool setFrom( const std::string & from_user ) ;
		// Sets the message envelope 'from'.
		// Returns false if an invalid user.

	bool addTo( const std::string & to_user ) ;
		// Adds an envelope 'to'.
		// Returns false if an invalid user.
		// Precondition: setFrom() called
		// since clear() or process().

	void addReceived( const std::string & ) ;
		// Adds a 'received' line to the
		// start of the content.
		// Precondition: at least one
		// successful addTo() call

	void addText( const std::string & ) ;
		// Adds text.
		// Precondition: at least one
		// successful addTo() call

	std::string process() ;
		// Processes and clears the message.
		// Returns a non-zero-length reason
		// string on error.

private:
	static bool isLocal( const std::string & ) ;
	static bool isValid( const std::string & ) ;
	static bool isPostmaster( std::string ) ;
	static std::string fullName( const std::string & ) ;
	ProtocolMessage( const ProtocolMessage & ) ;
	void operator=( const ProtocolMessage & ) ;

private:
	G::Strings m_to_list ;
	ProtocolMessageImp * m_imp ;
} ;

#endif

