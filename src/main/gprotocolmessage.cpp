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
// gprotocolmessage.cpp
//

#include "gdef.h"
#include "gsmtp.h"
#include "gprotocolmessage.h"
#include "gmessagestore.h"
#include "gmemory.h"
#include "gstr.h"
#include "gassert.h"
#include "glog.h"

// Class: GSmtp::ProtocolMessageImp
// Description: A private pimple-pattern implementation class for GSmtp::ProtocolMessage.
//
class GSmtp::ProtocolMessageImp
{
public:
	std::auto_ptr<NewMessage> m_msg ;
} ;

// ===

GSmtp::ProtocolMessage::ProtocolMessage() :
	m_imp(NULL)
{
	m_imp = new ProtocolMessageImp ;
}

GSmtp::ProtocolMessage::~ProtocolMessage()
{
	delete m_imp ;
}

void GSmtp::ProtocolMessage::clear()
{
	m_imp->m_msg <<= 0 ;
}

bool GSmtp::ProtocolMessage::setFrom( const std::string & from )
{
	try
	{
		if( from.length() == 0U )
			return false ;

		G_ASSERT( m_imp->m_msg.get() == NULL ) ;
		clear() ; // just in case
		std::auto_ptr<NewMessage> new_msg = MessageStore::instance().newMessage( from ) ;
		m_imp->m_msg = new_msg ;
		return true ;
	}
	catch( std::exception & e )
	{
		G_ERROR( "GSmtp::ProtocolMessage::setFrom: error: " << e.what() ) ;
		return false ;
	}
}

bool GSmtp::ProtocolMessage::addTo( const std::string & to )
{
	G_ASSERT( m_imp->m_msg.get() != NULL ) ;
	if( to.length() > 0U && m_imp->m_msg.get() != NULL )
	{
		bool is_local = isLocal(to) ;
		if( is_local && !isValid(to) )
		{
			G_WARNING( "GSmtp::ProtocolMessage: rejecting local recipent (not postmaster): " << to ) ;
			return false ;
		}
		else
		{
			m_imp->m_msg->addTo( to , is_local ) ;
			return true ;
		}
	}
	else
	{
		return false ;
	}
}

void GSmtp::ProtocolMessage::addReceived( const std::string & line )
{
	addText( line ) ;
}

void GSmtp::ProtocolMessage::addText( const std::string & line )
{
	G_ASSERT( m_imp->m_msg.get() != NULL ) ;
	if( m_imp->m_msg.get() != NULL )
		m_imp->m_msg->addText( line ) ;
}

std::string GSmtp::ProtocolMessage::process()
{
	try
	{
		G_ASSERT( m_imp->m_msg.get() != NULL ) ;
		if( m_imp->m_msg.get() != NULL )
		{
			m_imp->m_msg->store() ;
		}
		clear() ;
		return std::string() ;
	}
	catch( std::exception & e )
	{
		G_ERROR( "GSmtp::ProtocolMessage::process: error: " << e.what() ) ;
		clear() ;
		return std::string( e.what() ) ;
	}
}

std::pair<bool,std::string> GSmtp::ProtocolMessage::verify( const std::string & user )
{
	G_DEBUG( "GSmtp::ProtocolMessage::verify: \"" << user << "\"" ) ;
	std::pair<bool,std::string> rc( isLocal(user) , std::string() ) ;
	if( isLocal(user) && isValid(user) )
		rc.second = fullName(user) ;
	return rc ;
}

//static
bool GSmtp::ProtocolMessage::isLocal( const std::string & user )
{
	return user.find('@') == std::string::npos ;
}

//static
bool GSmtp::ProtocolMessage::isValid( const std::string & user )
{
	// only recognise one local mailbox
	return isPostmaster(user) ;
}

//static
bool GSmtp::ProtocolMessage::isPostmaster( std::string user )
{
	G::Str::toUpper( user ) ;
	G::Str::trim( user , " \t" ) ;
	return user == "POSTMASTER" ;
}

//static
std::string GSmtp::ProtocolMessage::fullName( const std::string & user )
{
	return "Local postmaster <postmaster@localhost>" ;
}

