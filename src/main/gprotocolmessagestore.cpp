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
// gprotocolmessagestore.cpp
//

#include "gdef.h"
#include "gsmtp.h"
#include "gprotocolmessagestore.h"
#include "gmessagestore.h"
#include "gmemory.h"
#include "gstr.h"
#include "gassert.h"
#include "glog.h"

GSmtp::ProtocolMessageStore::ProtocolMessageStore()
{
}

GSmtp::ProtocolMessageStore::~ProtocolMessageStore()
{
}

void GSmtp::ProtocolMessageStore::clear()
{
	m_msg <<= 0 ;
}

bool GSmtp::ProtocolMessageStore::setFrom( const std::string & from )
{
	try
	{
		if( from.length() == 0U )
			return false ;

		G_ASSERT( m_msg.get() == NULL ) ;
		clear() ; // just in case

		std::auto_ptr<NewMessage> new_message( MessageStore::instance().newMessage(from) ) ;
		m_msg <<= new_message.release() ;

		return true ;
	}
	catch( std::exception & e )
	{
		G_ERROR( "GSmtp::ProtocolMessage::setFrom: error: " << e.what() ) ;
		return false ;
	}
}

bool GSmtp::ProtocolMessageStore::addTo( const std::string & to , Verifier::Status to_status )
{
	G_ASSERT( m_msg.get() != NULL ) ;
	if( to.length() > 0U && m_msg.get() != NULL )
	{
		const bool is_local = to_status.first ;
		const bool is_valid = is_local && to_status.second.length() != 0U  ;
		if( is_local && !is_valid )
		{
			G_WARNING( "GSmtp::ProtocolMessage: rejecting local recipent (not postmaster): " << to ) ;
			return false ;
		}
		else
		{
			m_msg->addTo( to , is_local ) ;
			return true ;
		}
	}
	else
	{
		return false ;
	}
}

void GSmtp::ProtocolMessageStore::addReceived( const std::string & line )
{
	addText( line ) ;
}

void GSmtp::ProtocolMessageStore::addText( const std::string & line )
{
	G_ASSERT( m_msg.get() != NULL ) ;
	if( m_msg.get() != NULL )
		m_msg->addText( line ) ;
}

void GSmtp::ProtocolMessageStore::process( Callback & callback )
{
	try
	{
		G_ASSERT( m_msg.get() != NULL ) ;
		unsigned long id = 0UL ;
		if( m_msg.get() != NULL )
		{
			m_msg->store() ;
			id = m_msg->id() ;
		}
		clear() ;
		callback.processDone( true , id , std::string() ) ;
	}
	catch( std::exception & e )
	{
		G_ERROR( "GSmtp::ProtocolMessage::process: error: " << e.what() ) ;
		clear() ;
		callback.processDone( false , 0UL , e.what() ) ;
	}
}

