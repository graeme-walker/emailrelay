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
// gverifier.cpp
//

#include "gdef.h"
#include "gsmtp.h"
#include "gverifier.h"
#include "gstr.h"
#include "gassert.h"
#include "glog.h"

GSmtp::Verifier::Status GSmtp::Verifier::verify( const std::string & user ) const
{
	G_DEBUG( "GSmtp::ProtocolMessage::verify: \"" << user << "\"" ) ;
	Status rc( isLocal(user) , std::string() ) ;
	if( isLocal(user) && isValid(user) )
		rc.second = fullName(user) ;
	return rc ;
}

//static
bool GSmtp::Verifier::isLocal( const std::string & user )
{
	return user.find('@') == std::string::npos ;
}

//static
bool GSmtp::Verifier::isValid( const std::string & user )
{
	// only recognise one local mailbox
	return isPostmaster(user) ;
}

//static
bool GSmtp::Verifier::isPostmaster( std::string user )
{
	G::Str::toUpper( user ) ;
	G::Str::trim( user , " \t" ) ;
	return user == "POSTMASTER" ;
}

//static
std::string GSmtp::Verifier::fullName( const std::string & user )
{
	return "Local postmaster <postmaster@localhost>" ;
}

