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
// gserverprotocol.cpp
//

#include "gdef.h"
#include "gsmtp.h"
#include "gserverprotocol.h"
#include "gbase64.h"
#include "gdate.h"
#include "gtime.h"
#include "gdatetime.h"
#include "gstr.h"
#include "glog.h"
#include "gassert.h"
#include <string>

GSmtp::ServerProtocol::ServerProtocol( Sender & sender , Verifier & verifier , ProtocolMessage & pmessage ,
	const std::string & thishost , const std::string & peer_address ) :
		m_sender(sender) ,
		m_pmessage(pmessage) ,
		m_verifier(verifier) ,
		m_state(sStart) ,
		m_thishost(thishost) ,
		m_peer_address(peer_address) ,
		m_authenticated(false)
{
	// (dont send anything to the peer from this ctor -- the Sender
	// object is not fuly constructed)

	addTransition( eQuit    , s_Any   , sEnd     , &GSmtp::ServerProtocol::doQuit ) ;
	addTransition( eUnknown , s_Any   , s_Same   , &GSmtp::ServerProtocol::doUnknown ) ;
	addTransition( eRset    , s_Any   , sIdle    , &GSmtp::ServerProtocol::doRset ) ;
	addTransition( eNoop    , s_Any   , s_Same   , &GSmtp::ServerProtocol::doNoop ) ;
	addTransition( eVrfy    , s_Any   , s_Same   , &GSmtp::ServerProtocol::doVrfy ) ;
	addTransition( eEhlo    , s_Any   , sIdle    , &GSmtp::ServerProtocol::doEhlo , s_Same ) ;
	addTransition( eHelo    , s_Any   , sIdle    , &GSmtp::ServerProtocol::doHelo , s_Same ) ;
	addTransition( eMail    , sIdle   , sGotMail , &GSmtp::ServerProtocol::doMail , sIdle ) ;
	addTransition( eRcpt    , sGotMail, sGotRcpt , &GSmtp::ServerProtocol::doRcpt , sGotMail ) ;
	addTransition( eRcpt    , sGotRcpt, sGotRcpt , &GSmtp::ServerProtocol::doRcpt ) ;
	addTransition( eData    , sGotMail, sIdle    , &GSmtp::ServerProtocol::doNoRecipients ) ;
	addTransition( eData    , sGotRcpt, sData    , &GSmtp::ServerProtocol::doData ) ;

	if( m_sasl.active() )
	{
		addTransition( eAuth    , sStart  , sAuth    , &GSmtp::ServerProtocol::doAuth , sIdle ) ;
		addTransition( eAuth    , sIdle   , sAuth    , &GSmtp::ServerProtocol::doAuth , sIdle ) ;
		addTransition( eAuthData, sAuth   , sAuth    , &GSmtp::ServerProtocol::doAuthData , sIdle ) ;
	}
}

void GSmtp::ServerProtocol::init( const std::string & ident )
{
	sendGreeting( m_thishost , ident ) ;
}

void GSmtp::ServerProtocol::addTransition( Event e , State state_from , State state_to , Action action , State state_alt )
{
	if( state_alt == s_Same ) state_alt = state_to ;
	m_map.insert( Map::value_type( e , Transition(state_from,state_to,action,state_alt) ) ) ;
}

void GSmtp::ServerProtocol::sendGreeting( const std::string & thishost , const std::string & ident )
{
	std::stringstream ss ;
	ss << "220 " << thishost << " -- " << ident << " -- Service ready" ;
	send( ss.str() ) ;
}

bool GSmtp::ServerProtocol::apply( const std::string & line )
{
	if( m_state == sData )
	{
		if( isEndOfText(line) )
		{
			G_LOG( "GSmtp::ServerProtocol: rx<<: [message content not logged]" ) ;
			G_LOG( "GSmtp::ServerProtocol: rx<<: \"" << G::Str::toPrintableAscii(line) << "\"" ) ;
			m_state = sProcessing ;
			m_pmessage.process( *this , m_sasl.id() , m_peer_address ) ; // -> processDone() callback
		}
		else
		{
			m_pmessage.addText( isEscaped(line) ? line.substr(1U) : line ) ;
		}
		return false ;
	}
	else
	{
		Event event ;
		if( m_state == sAuth )
		{
			event = eAuthData ;
			G_LOG( "GSmtp::ServerProtocol: rx<<: [authentication response not logged]" ) ;
			G_DEBUG( "GSmtp::ServerProtocol: rx<<: {" << Base64::decode(line) << "}" ) ;
		}
		else
		{
			G_LOG( "GSmtp::ServerProtocol: rx<<: \"" << G::Str::toPrintableAscii(line) << "\"" ) ;
			event = commandEvent( commandWord(line) ) ;
		}

		State new_state = applyEvent( event , commandLine(line) ) ;
		return new_state == sEnd ;
	}
}

void GSmtp::ServerProtocol::processDone( bool success , unsigned long , const std::string & reason )
{
	G_ASSERT( m_state == sProcessing ) ; // (a RSET will call m_pmessage.clear() to cancel the callback)
	if( m_state == sProcessing ) // just in case
	{
		m_state = sIdle ;
		sendCompletionReply( success , reason ) ;
	}
}

GSmtp::ServerProtocol::State GSmtp::ServerProtocol::applyEvent( Event event , const std::string & line )
{
	// look up in the multimap keyed on current-state + event
	//
	Map::iterator p = m_map.find(event) ;
	for( ; p != m_map.end() && (*p).first == event ; ++p )
	{
		if( (*p).second.from == s_Any || (*p).second.from == m_state )
		{
			// change state
			//
			State old_state = m_state ;
			if( (*p).second.to != s_Same )
				m_state = (*p).second.to ;
			State state = m_state ;

			// perform action
			//
			bool predicate = true ;
			(this->*((*p).second.action))( line , predicate ) ;

			// respond to predicate -- note that
			// if the state is sEnd then we cannot
			// touch any member variables
			//
			if( state != sEnd && !predicate )
			{
				State alt_state = (*p).second.alt ;
				m_state = alt_state == s_Same ? old_state : alt_state ;
				state = m_state ;
			}
			return state ;
		}
	}
	sendOutOfSequence( line ) ;
	return m_state ;
}

void GSmtp::ServerProtocol::doQuit( const std::string & , bool & )
{
	sendClosing() ;
	m_sender.protocolDone() ;
	// do nothing more -- this object may have been deleted already
}

void GSmtp::ServerProtocol::doNoop( const std::string & , bool & )
{
	sendOk() ;
}

void GSmtp::ServerProtocol::doVrfy( const std::string & line , bool & )
{
	std::string mbox = parseMailbox( line ) ;
	Verifier::Status rc = m_verifier.verify( mbox ) ;
	bool local = rc.first ;
	if( local && rc.second.length() )
		sendVerified( rc.second ) ;
	else if( local )
		sendNotVerified( mbox ) ;
	else
		sendWillAccept( mbox ) ;
}

std::string GSmtp::ServerProtocol::parseMailbox( const std::string & line ) const
{
	std::string user ;
	size_t pos = line.find_first_of( " \t" ) ;
	if( pos != std::string::npos )
		user = line.substr(pos) ;

	G::Str::trim( user , " \t" ) ;
	return user ;
}

void GSmtp::ServerProtocol::doEhlo( const std::string & line , bool & predicate )
{
	std::string peer_name = parsePeerName( line ) ;
	if( peer_name.empty() )
	{
		predicate = false ;
		sendMissingParameter() ;
	}
	else
	{
		m_peer_name = peer_name ;
		m_pmessage.clear() ;
		sendEhloReply( m_thishost ) ;
	}
}

void GSmtp::ServerProtocol::doHelo( const std::string & line , bool & predicate )
{
	std::string peer_name = parsePeerName( line ) ;
	if( peer_name.empty() )
	{
		predicate = false ;
		sendMissingParameter() ;
	}
	else
	{
		m_peer_name = peer_name ;
		m_pmessage.clear() ;
		sendHeloReply( m_thishost ) ;
	}
}

void GSmtp::ServerProtocol::doAuth( const std::string & line , bool & predicate )
{
	G::Str::StringArray word_array ;
	G::Str::splitIntoTokens( line , word_array , " \t" ) ;

	std::string mechanism = word_array.size() > 1U ? word_array[1U] : std::string() ;
	G::Str::toUpper( mechanism ) ;
	std::string initial_response = word_array.size() > 2U ? word_array[2U] : std::string() ;
	bool got_initial_response = word_array.size() > 2U ;

	G_DEBUG( "ServerProtocol::doAuth: [" << mechanism << "], [" << initial_response << "]" ) ;

	if( m_authenticated )
	{
		G_WARNING( "GSmtp::ServerProtocol: too many AUTHs" ) ;
		predicate = false ; // => idle
		sendOutOfSequence(line) ; // see RFC2554 "Restrictions"
	}
	else if( ! m_sasl.init(mechanism) )
	{
		G_WARNING( "GSmtp::ServerProtocol: request for unsupported AUTH mechanism: " << mechanism ) ;
		predicate = false ; // => idle
		send( "504 Unsupported authentication mechanism" ) ;
	}
	else if( got_initial_response && ! Base64::valid(initial_response) )
	{
		G_WARNING( "GSmtp::ServerProtocol: invalid base64 encoding of AUTH parameter" ) ;
		predicate = false ; // => idle
		send( "501 Invalid argument" ) ;
	}
	else if( got_initial_response && m_sasl.mustChallenge() )
	{
		predicate = false ; // => idle
		sendAuthDone( false ) ;
	}
	else if( got_initial_response )
	{
		std::string s = initial_response == "=" ? std::string() : Base64::decode(initial_response) ;
		bool done = false ;
		std::string next_challenge = m_sasl.apply( s , done ) ;
		if( done )
		{
			predicate = false ; // => idle
			m_authenticated = m_sasl.authenticated() ;
			sendAuthDone( m_sasl.authenticated() ) ;
		}
		else
		{
			sendChallenge( next_challenge ) ;
		}
	}
	else
	{
		sendChallenge( m_sasl.initialChallenge() ) ;
	}
}

void GSmtp::ServerProtocol::sendAuthDone( bool ok )
{
	if( ok )
		send( "235 Authentication sucessful" ) ;
	else
		send( "535 Authentication failed" ) ;
}

void GSmtp::ServerProtocol::doAuthData( const std::string & line , bool & predicate )
{
	if( line == "*" )
	{
		predicate = false ; // => idle
		send( "501 authentication cancelled" ) ;
	}
	else if( ! Base64::valid(line) )
	{
		G_WARNING( "GSmtp::ServerProtocol: invalid base64 encoding of authentication response" ) ;
		predicate = false ; // => idle
		sendAuthDone( false ) ;
	}
	else
	{
		bool done = false ;
		std::string next_challenge = m_sasl.apply( Base64::decode(line) , done ) ;
		if( done )
		{
			predicate = false ; // => idle
			m_authenticated = m_sasl.authenticated() ;
			sendAuthDone( m_sasl.authenticated() ) ;
		}
		else
		{
			sendChallenge( next_challenge ) ;
		}
	}
}

void GSmtp::ServerProtocol::sendChallenge( const std::string & s )
{
	send( std::string("334 ") + Base64::encode(s) ) ;
}

void GSmtp::ServerProtocol::doMail( const std::string & line , bool & predicate )
{
	m_pmessage.clear() ;
	std::string from = parseFrom( line ) ;
	bool ok = m_pmessage.setFrom( from ) ;
	predicate = ok ;
	if( ok )
		sendMailReply() ;
	else
		sendBadFrom( from ) ;
}

void GSmtp::ServerProtocol::doRcpt( const std::string & line , bool & predicate )
{
	std::string to = parseTo( line ) ;
	bool ok = m_pmessage.addTo( to , m_verifier.verify(to) ) ;
	predicate = ok ;
	if( ok )
		sendRcptReply() ;
	else
		sendBadTo( to ) ;
}

void GSmtp::ServerProtocol::doUnknown( const std::string & line , bool & )
{
	sendUnrecognised( line ) ;
}

void GSmtp::ServerProtocol::doRset( const std::string & , bool & )
{
	m_pmessage.clear() ;
	m_authenticated = false ; // (not clear in the RFCs)
	sendRsetReply() ;
}

void GSmtp::ServerProtocol::doNoRecipients( const std::string & , bool & )
{
	sendNoRecipients() ;
}

void GSmtp::ServerProtocol::doData( const std::string & , bool & )
{
	m_pmessage.addReceived( receivedLine() ) ;
	sendDataReply() ;
}

void GSmtp::ServerProtocol::sendOutOfSequence( const std::string & )
{
	send( "503 command out of sequence -- use RSET to resynchronise" ) ;
}

void GSmtp::ServerProtocol::sendMissingParameter()
{
	send( "501 parameter required" ) ;
}

bool GSmtp::ServerProtocol::isEndOfText( const std::string & line ) const
{
	return line.length() == 1U && line.at(0U) == '.' ;
}

bool GSmtp::ServerProtocol::isEscaped( const std::string & line ) const
{
	return line.length() > 1U && line.at(0U) == '.' ;
}

std::string GSmtp::ServerProtocol::commandWord( const std::string & line_in ) const
{
	std::string line( line_in ) ;
	G::Str::trimLeft( line , " \t" ) ;

	size_t pos = line.find_first_of( " \t" ) ;
	std::string command = line.substr( 0U , pos ) ;

	G::Str::toUpper( command ) ;
	return command ;
}

std::string GSmtp::ServerProtocol::commandLine( const std::string & line_in ) const
{
	std::string line( line_in ) ;
	G::Str::trimLeft( line , " \t" ) ;
	return line ;
}

GSmtp::ServerProtocol::Event GSmtp::ServerProtocol::commandEvent( const std::string & command ) const
{
	if( command == "QUIT" ) return eQuit ;
	if( command == "HELO" ) return eHelo ;
	if( command == "EHLO" ) return eEhlo ;
	if( command == "RSET" ) return eRset ;
	if( command == "DATA" ) return eData ;
	if( command == "RCPT" ) return eRcpt ;
	if( command == "MAIL" ) return eMail ;
	if( command == "VRFY" ) return eVrfy ;
	if( command == "NOOP" ) return eNoop ;
	if( command == "HELP" ) return eHelp ;
	if( m_sasl.active() && command == "AUTH" ) return eAuth ;
	return eUnknown ;
}

void GSmtp::ServerProtocol::sendClosing()
{
	send( "221 closing connection" ) ;
}

void GSmtp::ServerProtocol::sendVerified( const std::string & user )
{
	send( std::string("250 ") + user ) ;
}

void GSmtp::ServerProtocol::sendNotVerified( const std::string & user )
{
	send( std::string("550 no such mailbox: ") + user ) ;
}

void GSmtp::ServerProtocol::sendWillAccept( const std::string & user )
{
	send( std::string("252 cannot verify but will accept: ") + user ) ;
}

void GSmtp::ServerProtocol::sendUnrecognised( const std::string & line )
{
	send( "500 command unrecognized: \"" + line + std::string("\"") ) ;
}

void GSmtp::ServerProtocol::sendNoRecipients()
{
	send( "554 no valid recipients" ) ;
}

void GSmtp::ServerProtocol::sendDataReply()
{
	send( "354 start mail input -- end with <CRLF>.<CRLF>" ) ;
}

void GSmtp::ServerProtocol::sendRsetReply()
{
	send( "250 state reset" ) ;
}

void GSmtp::ServerProtocol::sendMailReply()
{
	sendOk() ;
}

void GSmtp::ServerProtocol::sendCompletionReply( bool ok , const std::string & reason )
{
	if( ok )
		sendOk() ;
	else
		send( std::string("452 message processing failed: ") + reason ) ;
}

void GSmtp::ServerProtocol::sendRcptReply()
{
	sendOk() ;
}

void GSmtp::ServerProtocol::sendBadFrom( const std::string & /*from*/ )
{
	send( "553 mailbox name not allowed" ) ;
}

void GSmtp::ServerProtocol::sendBadTo( const std::string & to )
{
	send( std::string("550 mailbox unavailable: ") + to ) ;
}

void GSmtp::ServerProtocol::sendEhloReply( const std::string & domain )
{
	std::stringstream ss ;
		ss << "250-" << domain << " says hello" << crlf() ;
	if( m_sasl.active() )
		ss << "250-AUTH " << m_sasl.mechanisms() << crlf() ;
		ss << "250 8BITMIME" ;
	send( ss.str() ) ;
}

void GSmtp::ServerProtocol::sendHeloReply( const std::string & /*domain*/ )
{
	sendOk() ;
}

void GSmtp::ServerProtocol::sendOk()
{
	send( "250 OK" ) ;
}

//static
std::string GSmtp::ServerProtocol::crlf()
{
	return std::string( "\015\012" ) ;
}

void GSmtp::ServerProtocol::send( std::string line )
{
	G_LOG( "GSmtp::ServerProtocol: tx>>: \"" << line << "\"" ) ;
	line.append( crlf() ) ;
	m_sender.protocolSend( line ) ;
}

GSmtp::ServerProtocol::~ServerProtocol()
{
}

std::string GSmtp::ServerProtocol::parseFrom( const std::string & line ) const
{
	// eg. MAIL FROM:<me@localhost>
	return parse( line ) ;
}

std::string GSmtp::ServerProtocol::parseTo( const std::string & line ) const
{
	// eg. RCPT TO:<@first.co.uk,@second.co.uk:you@final.co.uk>
	// eg. RCPT TO:<Postmaster>
	return parse( line ) ;
}

std::string GSmtp::ServerProtocol::parse( const std::string & line ) const
{
	size_t start = line.find( '<' ) ;
	size_t end = line.find( '>' ) ;
	if( start == std::string::npos || end == std::string::npos || end < start )
		return std::string() ;

	std::string s = line.substr( start + 1U , end - start - 1U ) ;
	G::Str::trim( s , " \t" ) ;

	// strip source route
	if( s.length() > 0U && s.at(0U) == '@' )
	{
		size_t colon_pos = s.find( ':' ) ;
		if( colon_pos == std::string::npos )
			return std::string() ;
		s = s.substr( colon_pos + 1U ) ;
	}

	return s ;
}

std::string GSmtp::ServerProtocol::parsePeerName( const std::string & line ) const
{
	size_t pos = line.find_first_of( " \t" ) ;
	if( pos == std::string::npos )
		return std::string() ;

	std::string peer_name = line.substr( pos + 1U ) ;
	G::Str::trim( peer_name , " \t" ) ;
	return peer_name ;
}

std::string GSmtp::ServerProtocol::receivedLine() const
{
	G::DateTime::EpochTime t = G::DateTime::now() ;
	G::DateTime::BrokenDownTime tm = G::DateTime::local(t) ;
	std::string zone = G::DateTime::offsetString(G::DateTime::offset(t)) ;
	G::Date date( tm ) ;
	G::Time time( tm ) ;

	std::stringstream ss ;
	ss
		<< "Reveived: "
		<< "FROM " << m_peer_name << " "
		<< "([" << m_peer_address << "]) "
		<< "BY " << m_thishost << " "
		<< "WITH ESMTP "
		<< "; "
		<< date.weekdayString(true) << ", "
		<< date.monthday() << " "
		<< date.monthString(true) << " "
		<< date.year() << " "
		<< time.hhmmss(":") << " "
		<< zone ;
	return ss.str() ;
}

// ===

GSmtp::ServerProtocol::Sender::~Sender()
{
}

// ===

GSmtp::ServerProtocol::Transition::Transition( State s1 , State s2 , Action a , State s3 ) :
	from(s1) ,
	to(s2) ,
	action(a) ,
	alt(s3)
{
}

