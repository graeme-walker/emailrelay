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
#include <string>

namespace GSmtp
{
	class Verifier ;
} ;

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
		bool is_local ;
		std::string full_name ;
		std::string address ;
	} ;

        Status verify( const std::string & recipient_address ) const ;
		// Checks a recipient address returning
		// a structure which indicates whether the
		// address is local, what the full name is,
		// and the canonical address.
		//
		// If syntactically local then 'is_local' is
		// set true. If local and valid then
		// 'full_name' is set to the full description
		// and 'address' is set to the
		// canonical local address (without an
		// at sign).
		//
		// If syntactically remote, then 'is_local'
		// is set false, 'full_name' is empty,
		// and 'address' is copied from
		// 'recipient_address'.
} ;

#endif
