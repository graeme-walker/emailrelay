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
// gverifier.cpp
//

#include "gdef.h"
#include "gsmtp.h"
#include "gverifier.h"
#include "gstr.h"
#include "glocal.h"
#include "gassert.h"
#include "glog.h"

GSmtp::Verifier::Status GSmtp::Verifier::verify( const std::string & address ) const
{
	G_DEBUG( "GSmtp::ProtocolMessage::verify: \"" << address << "\"" ) ;

	std::string fqdn = GNet::Local::fqdn() ;
	std::string host ;
	std::string user( address ) ;
	size_t at_pos = address.find('@') ;
	if( at_pos != std::string::npos )
	{
		host = address.substr(at_pos+1U) ;
		user = address.substr(0U,at_pos) ;
	}
	G::Str::toUpper( fqdn ) ;
	G::Str::toUpper( host ) ;
	G::Str::toUpper( user ) ;

	Status status ;
	status.is_local = false ;
	status.address = address ;
	if( user == "POSTMASTER" && ( host.empty() || host == "LOCALHOST" || host == fqdn ) )
	{
		// accept 'postmaster' for local delivery
		status.is_local = true ;
		status.full_name = "Local postmaster <postmaster@localhost>" ;
		status.address = "postmaster" ;
	}
	else if( host.empty() || host == "LOCALHOST" )
	{
		// reject local addressees
		status.is_local = true ;
	}
	return status ;
}

