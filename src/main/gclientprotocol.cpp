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
// gclientprotocol.cpp
//

#include "gdef.h"
#include "gnet.h"
#include "gsmtp.h"
#include "glocal.h"
#include "gfile.h"
#include "gstr.h"
#include "gmemory.h"
#include "gclientprotocol.h"
#include "gresolve.h"
#include "glog.h"
#include "gassert.h"

GSmtp::ClientProtocol::ClientProtocol( Sender & sender , const std::string & thishost ) :
	m_state(sStart) ,
	m_thishost(thishost) ,
	m_sender(sender) ,
	m_callback(NULL) ,
	m_server_has_8bitmime(false) ,
	m_said_hello(false)
{
}

void GSmtp::ClientProtocol::start( const std::string & from , const G::Strings & to , bool eight_bit ,
	std::auto_ptr<std::istream> content , Callback & callback )
{
	G_DEBUG( "GSmtp::ClientProtocol::start" ) ;
	m_to = to ;
	m_from = from ;
	m_content = content ;
	m_callback = &callback ;
	m_server_has_8bitmime = false ;
	m_message_is_8bit = eight_bit ;
	m_reply = Reply() ;
	if( m_state != sStart && m_state != sEnd )
		throw NotReady() ;

	if( m_said_hello )
	{
		m_state = sSentMail ;
		sendMail() ;
	}
	else
	{
		m_state = sSentEhlo ;
		send( std::string("EHLO ") + m_thishost ) ;
	}
}

bool GSmtp::ClientProtocol::done() const
{
	return m_state == sEnd ;
}

void GSmtp::ClientProtocol::sendDone()
{
	if( m_state == sData )
	{
		size_t n = 0U ;
		while( sendLine() )
			n++ ;

		G_LOG( "GSmtp::ClientProtocol: tx>>: [" << n << " line(s) of content]" ) ;
		if( endOfContent() )
		{
			m_state = sDone ;
			send(".",true) ;
		}
	}
}

//static
bool GSmtp::ClientProtocol::parseReply( Reply & stored_reply , const std::string & rx , std::string & reason )
{
	Reply this_reply = Reply( rx ) ;
	if( ! this_reply.validFormat() )
	{
		stored_reply = Reply() ;
		reason = "invalid reply format" ;
		return false ;
	}
	else if( stored_reply.validFormat() && stored_reply.incomplete() )
	{
		if( ! stored_reply.add(this_reply) )
		{
			stored_reply = Reply() ;
			reason = "invalid continuation line" ;
			return false ;
		}
	}
	else
	{
		stored_reply = this_reply ;
	}
	return ! stored_reply.incomplete() ;
}

void GSmtp::ClientProtocol::apply( const std::string & rx )
{
	G_LOG( "GSmtp::ClientProtocol: rx<<: \"" << G::Str::toPrintableAscii(rx) << "\"" ) ;

	std::string reason ;
	bool complete = parseReply( m_reply , rx , reason ) ;
	if( complete )
	{
		applyEvent( m_reply ) ;
	}
	else
	{
		if( reason.length() != 0U )
			send( std::string("550 syntax error: ")+reason ) ;
	}
}

void GSmtp::ClientProtocol::sendMail()
{
	std::string mail_from = std::string("MAIL FROM:<") + m_from + ">" ;
	if( m_server_has_8bitmime )
	{
		mail_from.append( " BODY=8BITMIME" ) ;
	}
	else if( m_message_is_8bit )
	{
		throw NarrowPipe() ; // (could do better)
	}
	send( mail_from ) ;
}

void GSmtp::ClientProtocol::applyEvent( const Reply & reply )
{
	if( reply.is(Reply::ServiceReady_220) )
	{
		; // no-op
	}
	else if( m_state == sReset )
	{
		m_state = sStart ;
		m_said_hello = false ;
	}
	else if( m_state == sStart )
	{
		;
	}
	else if( m_state == sSentEhlo && reply.is(Reply::SyntaxError_500) )
	{
		m_state = sSentHelo ;
		send( std::string("HELO ") + m_thishost ) ;
	}
	else if( (m_state==sSentEhlo || m_state==sSentHelo) && reply.is(Reply::Ok_250) )
	{
		m_server_has_8bitmime = m_state == sSentEhlo && reply.textContains("8BITMIME") ;
		m_said_hello = true ;

		m_state = sSentMail ;
		sendMail() ;
	}
	else if( m_state == sSentMail && reply.is(Reply::Ok_250) )
	{
		if( m_to.size() == 0U )
		{
			// should never get here -- messages with no remote recipients
			// are filtered out by the message store
			throw NoRecipients() ;
		}

		m_state = sSentRcpt ;
		send( std::string("RCPT TO:<") + m_to.front() + std::string(">") ) ;
		m_to.pop_front() ;
	}
	else if( m_state == sSentRcpt && m_to.size() != 0U && reply.positive() )
	{
		send( std::string("RCPT TO:<") + m_to.front() + std::string(">") ) ;
		m_to.pop_front() ;
	}
	else if( m_state == sSentRcpt && reply.positive() )
	{
		m_state = sSentData ;
		send( std::string("DATA") ) ;
	}
	else if( m_state == sSentRcpt )
	{
		G_WARNING( "GSmtp::ClientProtocol: recipient rejected" ) ;
		m_state = sEnd ;
		doCallback( false , reply.text() ) ;
	}
	else if( m_state == sSentData && reply.is(Reply::OkForData_354) )
	{
		m_state = sData ;

		size_t n = 0U ;
		while( sendLine() )
			n++ ;

		G_LOG( "GSmtp::ClientProtocol: tx>>: [" << n << " line(s) of content]" ) ;
		if( endOfContent() )
		{
			m_state = sDone ;
			send( "." , true ) ;
		}
	}
	else if( m_state == sDone )
	{
		const bool ok = reply.is(Reply::Ok_250) ;
		m_state = sEnd ;
		doCallback( ok , ok ? std::string() : reply.text() ) ;
	}
	else
	{
		G_WARNING( "GSmtp::ClientProtocol: failure in client protocol: " << static_cast<int>(m_state) ) ;
		m_state = sEnd ; // (was sReset)
		send( "RSET" ) ; // for good meausre
		doCallback( false , std::string("unexpected response: ")+reply.text() ) ;
	}
}

void GSmtp::ClientProtocol::doCallback( bool ok , const std::string & reason )
{
	m_content <<= 0 ;
	if( m_callback )
	{
		Callback * cb = m_callback ;
		m_callback = NULL ;
		cb->protocolDone( ok , reason ) ;
	}
}

bool GSmtp::ClientProtocol::sendLine()
{
	std::string line = G::Str::readLineFrom( *(m_content.get()) , crlf() ) ;
	if( m_content->good() )
	{
		return send( line , false , false ) ;
	}
	else
	{
		return false ;
	}
}

bool GSmtp::ClientProtocol::endOfContent() const
{
	return !m_content->good() ;
}

bool GSmtp::ClientProtocol::send( const std::string & line , bool eot , bool log )
{
	if( log )
		G_LOG( "GSmtp::ClientProtocol: tx>>: \"" << G::Str::toPrintableAscii(line) << "\"" ) ;

	if( !eot && line.length() && line.at(0U) == '.' )
		return m_sender.protocolSend( std::string(".")+line+crlf() ) ;
	else
		return m_sender.protocolSend( line + crlf() ) ;
}

//static
std::string GSmtp::ClientProtocol::crlf()
{
	return std::string("\015\012") ;
}

// ===

GSmtp::ClientProtocolReply::ClientProtocolReply( const std::string & line ) :
	m_valid(false) ,
	m_complete(false)
{
	if( line.length() >= 3U &&
		is_digit(line.at(0U)) &&
		line.at(0U) <= '5' &&
		is_digit(line.at(1U)) &&
		is_digit(line.at(2U)) &&
		( line.length() == 3U || line.at(3U) == ' ' || line.at(3U) == '-' ) )
	{
		m_valid = true ;
		m_complete = line.length() == 3U || line.at(3U) == ' ' ;
		m_value = G::Str::toUInt( line.substr(0U,3U) ) ;
		if( line.length() > 3U )
		{
			m_text = line.substr(3U) ;
			G::Str::trimLeft( m_text , " \t" ) ;
		}
	}
}

bool GSmtp::ClientProtocolReply::validFormat() const
{
	return m_valid ;
}

bool GSmtp::ClientProtocolReply::incomplete() const
{
	return ! m_complete ;
}

bool GSmtp::ClientProtocolReply::positive() const
{
	return m_valid && m_value < 400U ;
}

unsigned int GSmtp::ClientProtocolReply::value() const
{
	return m_valid ? m_value : 0 ;
}

bool GSmtp::ClientProtocolReply::is( Value v ) const
{
	return value() == static_cast<unsigned int>( v ) ;
}

std::string GSmtp::ClientProtocolReply::text() const
{
	return m_text ;
}

//static
bool GSmtp::ClientProtocolReply::is_digit( char c )
{
	return c >= '0' && c <= '9' ;
}

GSmtp::ClientProtocolReply::Type GSmtp::ClientProtocolReply::type() const
{
	G_ASSERT( m_valid && (m_value/100U) >= 1U && (m_value/100U) <= 5U ) ;
	return static_cast<Type>( m_value / 100U ) ;
}

GSmtp::ClientProtocolReply::SubType GSmtp::ClientProtocolReply::subType() const
{
	unsigned int n = ( m_value / 10U ) % 10U ;
	if( n < 4U )
		return static_cast<SubType>( n ) ;
	else
		return Invalid_SubType ;
}

bool GSmtp::ClientProtocolReply::add( const ClientProtocolReply & other )
{
	G_ASSERT( other.m_valid ) ;
	G_ASSERT( m_valid ) ;
	G_ASSERT( !m_complete ) ;

	m_complete = other.m_complete ;
	m_text.append( std::string("\n") + other.text() ) ;
	return value() == other.value() ;
}

bool GSmtp::ClientProtocolReply::textContains( std::string key ) const
{
	G::Str::toUpper(key) ;
	return m_text.find(key) != std::string::npos ;
}

// ===

GSmtp::ClientProtocol::Sender::~Sender()
{
}

// ===

GSmtp::ClientProtocol::Callback::~Callback()
{
}

