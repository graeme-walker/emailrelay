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
// gverifier.h
//

#ifndef G_SMTP_VERIFIER_H
#define G_SMTP_VERIFIER_H

#include "gdef.h"
#include "gsmtp.h"
#include "gpath.h"
#include <string>

namespace GSmtp
{
	class Verifier ;
}

// Class: GSmtp::Verifier
// Description: A class which verifies recipient addresses.
// This functionality is used in the VRFY and RCPT commands
// in the SMTP server-side protocol.
// See also: ServerProtocol
//
class GSmtp::Verifier
{
public:
	struct Status
	{
		bool is_valid ;
		bool is_local ;
		std::string full_name ;
		std::string address ;
		std::string reason ;
	} ;

	explicit Verifier( const G::Path & exe ) ;
		// Constructor.

        Status verify( const std::string & recipient_address , const std::string & from = std::string() ) const ;
		// Checks a recipient address returning
		// a structure which indicates whether the
		// address is local, what the full name is,
		// and the canonical address.
		//
		// If invalid then 'is_valid' is set false
		// and a 'reason' is supplied.
		//
		// If valid and syntactically local then
		// 'is_local' is set true, 'full_name' is
		// set to the full description
		// and 'address' is set to the
		// canonical local address (without an
		// at sign).
		//
		// If valid and syntactically remote, then
		// 'is_local' is set false, 'full_name' is
		// empty, and 'address' is copied from
		// 'recipient_address'.
		//
		// The 'from' address is passed in for
		// RCPT commands, but not VRFY.

private:
	Status verifyInternal( const std::string & , const std::string & , const std::string & ,
		const std::string & , const std::string & ) const ;
	Status verifyExternal( const std::string & , const std::string & , const std::string & ,
		const std::string & , const std::string & ) const ;

private:
	G::Path m_path ;
} ;

#endif
