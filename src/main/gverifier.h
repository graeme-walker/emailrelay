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
	typedef std::pair<bool,std::string> Status ;

        std::pair<bool,std::string> verify( const std::string & recipient_address ) const ;
		// Checks a recipient address returning
		// <is-local>|<local-full-name>.
		//
		// If syntactically local then 'first' is
		// returned true. If local and valid then
		// 'second' is set to the full description.
		// If syntactically remote, then 'first'
		// is returned false and 'second' is empty.

private:
	static bool isLocal( const std::string & user ) ;
	static bool isValid( const std::string & user ) ;
	static bool isPostmaster( std::string user ) ;
	static std::string fullName( const std::string & user ) ;
} ;

#endif
