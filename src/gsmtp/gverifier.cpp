//
// Copyright (C) 2001-2003 Graeme Walker <graeme_walker@users.sourceforge.net>
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
#include "gstrings.h"
#include "gverifier.h"
#include "gprocess.h"
#include "gfile.h"
#include "groot.h"
#include "gstr.h"
#include "glocal.h"
#include "gassert.h"
#include "glog.h"

GSmtp::Verifier::Verifier( const G::Path & path ) :
	m_path(path)
{
}

GSmtp::Verifier::Status GSmtp::Verifier::verify( const std::string & address , const std::string & from ) const
{
	G_DEBUG( "GSmtp::ProtocolMessage::verify: to \"" << address << "\": from \"" << from << "\"" ) ;

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

	Status status =
		m_path == G::Path() ?
			verifyInternal( address , user , host , fqdn , from ) :
			verifyExternal( address , user , host , fqdn , from ) ;

	return status ;
}

GSmtp::Verifier::Status GSmtp::Verifier::verifyInternal( const std::string & address , const std::string & user ,
	const std::string & host , const std::string & fqdn , const std::string & ) const
{
	Status status ;
	if( user == "POSTMASTER" && ( host.empty() || host == "LOCALHOST" || host == fqdn ) )
	{
		// accept 'postmaster' for local delivery
		status.is_valid = true ;
		status.is_local = true ;
		status.full_name = "Local postmaster <postmaster@localhost>" ;
		status.address = "postmaster" ;
	}
	else if( host.empty() || host == "LOCALHOST" )
	{
		// reject local addressees
		status.is_valid = false ;
		status.is_local = true ;
		status.reason = "invalid local mailbox (not postmaster)" ;
	}
	else
	{
		// accept remote/fqdn addressees
		status.is_valid = true ;
		status.is_local = false ;
		status.address = address ;
	}
	return status ;
}

GSmtp::Verifier::Status GSmtp::Verifier::verifyExternal( const std::string & address , const std::string & user ,
	const std::string & host , const std::string & fqdn , const std::string & from ) const
{
	G::Strings args ;
	args.push_back( address ) ;
	args.push_back( user ) ;
	args.push_back( host ) ;
	args.push_back( fqdn ) ;
	args.push_back( from ) ;
	G_LOG( "GSmtp::Verifier: executing " << m_path << " " << address << " " << user << " " << host << " " << fqdn << " " << from ) ;

	std::string response ;
	int rc = G::Process::spawn( G::Root::nobody() , m_path , args , &response ) ;

	G::Str::trim( response , "\r\n\t" ) ;
	G_LOG( "GSmtp::Verifier: " << rc << ": \"" << response << "\"" ) ;
	G::Strings response_parts ;
	G::Str::splitIntoFields( response , response_parts , "\n" ) ;

	Status status ;
	if( ( rc == 0 || rc == 1 ) && response_parts.size() >= 2 )
	{
		status.is_valid = true ;
		status.is_local = rc == 0 ;
		status.full_name = response_parts.front() ;
		response_parts.pop_front() ;
		status.address = response_parts.front() ;
	}
	else
	{
		status.is_valid = false ;
		status.reason = response.empty() ? G::Str::fromInt(rc) : response ;
		G::Str::replaceAll( status.reason , "\n" , " " ) ;
	}
	return status ;
}
