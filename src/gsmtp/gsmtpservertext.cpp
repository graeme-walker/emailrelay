//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gsmtpservertext.cpp
///

#include "gdef.h"
#include "gsmtpservertext.h"
#include "gdate.h"
#include "gtime.h"
#include "gdatetime.h"
#include "gstr.h"
#include "glog.h"
#include "gassert.h"
#include <string>

GSmtp::ServerText::ServerText( const std::string & code_ident , bool anonymous , bool with_received_line ,
	const std::string & domain , const GNet::Address & peer_address ) :
		m_code_ident(code_ident) ,
		m_anonymous(anonymous) ,
		m_with_received_line(with_received_line) ,
		m_domain(domain) ,
		m_peer_address(peer_address)
{
	G_ASSERT( !domain.empty() ) ;
	if( domain.empty() )
		m_domain = "localhost" ; // never gets here
}

std::string GSmtp::ServerText::greeting() const
{
	std::string greeting_domain = m_domain ;
	return
		m_anonymous ?
			std::string("greeting") :
			greeting_domain.append(" -- ",4U).append(m_code_ident).append(" -- Service ready") ;
}

std::string GSmtp::ServerText::hello( const std::string & ) const
{
	std::string greeting_domain = m_domain ;
	return
		m_anonymous ?
			std::string("smtp says hello") :
			greeting_domain.append(" says hello") ;
}

std::string GSmtp::ServerText::received( const std::string & smtp_peer_name ,
	bool authenticated , bool secure , const std::string & protocol , const std::string & cipher ) const
{
	return m_with_received_line ? receivedLine( smtp_peer_name , m_peer_address.hostPartString() , m_domain ,
		authenticated , secure , protocol , cipher ) : std::string() ;
}

std::string GSmtp::ServerText::receivedLine( const std::string & smtp_peer_name ,
	const std::string & peer_address , const std::string & receivedline_domain ,
	bool authenticated , bool secure , const std::string & , const std::string & cipher_in )
{
	auto t = G::SystemTime::now() ;
	const G::BrokenDownTime tm = t.local() ;
	const std::string zone = G::Zone::offsetString(G::Zone::offset(t)) ;
	const G::Date date( tm ) ;
	const G::Time time( tm ) ;
	const std::string esmtp = std::string("ESMTP") + (secure?"S":"") + (authenticated?"A":"") ; // RFC-3848
	const std::string peer_name = G::Str::toPrintableAscii(
		G::Str::replaced(smtp_peer_name,' ','-') ) ; // typically alphanumeric with ".-:[]_"
	std::string cipher = secure ?
		G::Str::only(G::Str::alnum_(),G::Str::replaced(cipher_in,'-','_')) :
		std::string() ;

	// RFC-5321 4.4
	std::ostringstream ss ;
	ss
		<< "Received: from " << peer_name
		<< " ("
			<< "[" << peer_address << "]"
		<< ") by " << receivedline_domain << " with " << esmtp
		<< (cipher.empty()?"":" tls ") << cipher // RFC-8314 4.3 7.4
		<< " ; "
		<< date.weekdayName(true) << ", "
		<< date.monthday() << " "
		<< date.monthName(true) << " "
		<< date.yyyy() << " "
		<< time.hhmmss(":") << " "
		<< zone ;
	return ss.str() ;
}

