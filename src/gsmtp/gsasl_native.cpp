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
// gsasl_native.cpp
//

#include "gdef.h"
#include "gnet.h"
#include "gsmtp.h"
#include "glocal.h"
#include "gsasl.h"
#include "gstr.h"
#include "gmd5.h"
#include "gdatetime.h"
#include "gmemory.h"
#include "gdebug.h"
#include <sstream>

namespace
{
	const char * login_challenge_1 = "Username:" ;
	const char * login_challenge_2 = "Password:" ;
}

// ===

// Class: GSmtp::SaslServerImp
// Description: A private pimple-pattern implementation class used by GSmtp::SaslServer.
//
class GSmtp::SaslServerImp
{
public:
	bool m_first ;
	std::string m_mechanism ;
	std::string m_challenge ;
	bool m_authenticated ;
	std::string m_id ;
	SaslServerImp() ;
	void init( const std::string & mechanism ) ;
	bool validate( const std::string & secret , const std::string & response ) const ;
	static std::string clientResponse( const std::string & secret ,
		const std::string & challenge , bool & error ) ;
} ;

GSmtp::SaslServerImp::SaslServerImp() :
	m_first(true) ,
	m_authenticated(false)
{
}

void GSmtp::SaslServerImp::init( const std::string & mechanism )
{
	m_mechanism = mechanism ;
	m_authenticated = false ;
	m_id = std::string() ;
	m_first = true ;
	m_challenge = std::string() ;

	if( m_mechanism == "CRAM-MD5" )
	{
		std::ostringstream ss ;
		ss << "<" << ::rand() << "." << G::DateTime::now() << "@" << GNet::Local::fqdn() << ">" ;
		m_challenge = ss.str() ;
	}
}

bool GSmtp::SaslServerImp::validate( const std::string & secret , const std::string & response ) const
{
	try
	{
		G_ASSERT( m_mechanism == "CRAM-MD5" ) ;
		std::string hash = G::Md5::printable(G::Md5::hmac(secret,m_challenge,G::Md5::Masked())) ;
		return response == hash ;
	}
	catch( std::exception & e )
	{
		G_WARNING( "GSmtp::SaslServer: exception: " << e.what() ) ;
		return false ;
	}
	catch(...)
	{
		G_WARNING( "GSmtp::SaslServer: exception" ) ;
		return false ;
	}
}

std::string GSmtp::SaslServerImp::clientResponse( const std::string & secret ,
	const std::string & challenge , bool & error )
{
	try
	{
		return G::Md5::printable(G::Md5::hmac(secret,challenge,G::Md5::Masked())) ;
	}
	catch( std::exception & e )
	{
		G_WARNING( "GSmtp::SaslClient: " << e.what() ) ;
		error = true ;
	}
	catch(...)
	{
		error = true ;
	}
	return std::string() ;
}

// ===

std::string GSmtp::SaslServer::mechanisms( char c ) const
{
	std::string sep( 1U , c ) ;
	std::string s = std::string() + "LOGIN" + sep + "CRAM-MD5" ;
	return s ;
}

GSmtp::SaslServer::SaslServer() :
	m_imp(new SaslServerImp)
{
}

bool GSmtp::SaslServer::active() const
{
	return Sasl::instance().serverSecrets().valid() ;
}

GSmtp::SaslServer::~SaslServer()
{
	delete m_imp ;
}

bool GSmtp::SaslServer::mustChallenge() const
{
	return false ;
}

bool GSmtp::SaslServer::init( const std::string & mechanism )
{
	m_imp->init( mechanism ) ;

	G_DEBUG( "GSmtp::SaslServer::init: mechanism \"" << m_imp->m_mechanism << "\"" ) ;
	return m_imp->m_mechanism == "LOGIN" || m_imp->m_mechanism == "CRAM-MD5" ;
}

std::string GSmtp::SaslServer::initialChallenge() const
{
	if( m_imp->m_mechanism == "LOGIN" )
		return login_challenge_1 ;
	else
		return m_imp->m_challenge ;
}

std::string GSmtp::SaslServer::apply( const std::string & response , bool & done )
{
	done = false ;
	G_DEBUG( "GSmtp::SaslServer::apply: \"" << response << "\"" ) ;
	std::string next_challenge ;
	if( m_imp->m_mechanism == "CRAM-MD5" )
	{
		G::Strings part_list ;
		G::Str::splitIntoTokens( response , part_list , " " ) ;
		G_DEBUG( "GSmtp::SaslServer::apply: " << part_list.size() << " part(s)" ) ;
		if( part_list.size() == 2U )
		{
			m_imp->m_id = part_list.front() ;
			G_DEBUG( "GSmtp::SaslServer::apply: id \"" << m_imp->m_id << "\"" ) ;
			std::string secret = Sasl::instance().serverSecrets().secret(m_imp->m_mechanism,m_imp->m_id) ;
			m_imp->m_authenticated = m_imp->validate( secret , part_list.back() ) ;
		}
		done = true ;
	}
	else if( m_imp->m_first )
	{
		m_imp->m_first = false ;
		m_imp->m_id = response ;
		if( !m_imp->m_id.empty() )
			next_challenge = login_challenge_2 ;
	}
	else
	{
		std::string secret = Sasl::instance().serverSecrets().secret(m_imp->m_mechanism,m_imp->m_id) ;
		m_imp->m_first = true ;
		m_imp->m_authenticated = !response.empty() && response == secret ;
		done = true ;
	}

	G_DEBUG( "GSmtp::SaslServer::apply: \"" << response << "\" -> \"" << next_challenge << "\"" ) ;
	return next_challenge ;
}

bool GSmtp::SaslServer::authenticated() const
{
	return m_imp->m_authenticated ;
}

std::string GSmtp::SaslServer::id() const
{
	return m_imp->m_authenticated ? m_imp->m_id : std::string() ;
}

// ===

GSmtp::SaslClient::SaslClient( const std::string & server_name ) :
	m_imp(NULL)
{
	G_DEBUG( "GSmtp::SaslClient::ctor: \"" << server_name << "\"" ) ;
	(void) server_name.length() ; // pacify compiler
}

GSmtp::SaslClient::~SaslClient()
{
}

bool GSmtp::SaslClient::active() const
{
	return Sasl::instance().clientSecrets().valid() ;
}

std::string GSmtp::SaslClient::response( const std::string & mechanism ,
	const std::string & challenge , bool & done , bool & error ) const
{
	done = false ;
	error = false ;

	std::string rsp ;
	if( mechanism == "CRAM-MD5" )
	{
		std::string id = Sasl::instance().clientSecrets().id(mechanism) ;
		std::string secret = Sasl::instance().clientSecrets().secret(mechanism) ;
		error = id.empty() || secret.empty() ;
		if( !error )
			rsp = id + " " + SaslServerImp::clientResponse( secret , challenge , error ) ;

		done = true ;
	}
	else if( challenge == login_challenge_1 )
	{
		rsp = Sasl::instance().clientSecrets().id(mechanism) ;
		error = rsp.empty() ;
		done = false ;
	}
	else if( challenge == login_challenge_2 )
	{
		rsp = Sasl::instance().clientSecrets().secret(mechanism) ;
		error = rsp.empty() ;
		done = true ;
	}
	else
	{
		G_WARNING( "GSmtp::SaslClient: invalid challenge" ) ;
		done = true ;
		error = true ;
	}
	G_DEBUG( "GSmtp::SaslClient::response: \"" << mechanism << "\", \"" << challenge << "\" -> \"" << rsp << "\"" ) ;
	return rsp ;
}

std::string GSmtp::SaslClient::preferred( const G::Strings & mechanism_list ) const
{
	// short-circuit if no secrets
	//
	if( !active() )
		return std::string() ;

	// look for cram-md5 and login in the list
	//
	const std::string login( "LOGIN" ) ;
	const std::string cram( "CRAM-MD5" ) ;
	bool has_login = false ;
	bool has_cram = false ;
	for( G::Strings::const_iterator p = mechanism_list.begin() ; p != mechanism_list.end() ; ++p )
	{
		std::string mechanism = *p ;
		G::Str::toUpper( mechanism ) ;
		G_DEBUG( "GSmtp::SaslClient::preferred: \"" << mechanism << "\"" ) ;
		if( mechanism == login )
			has_login = true ;
		else if( mechanism == cram )
			has_cram = true ;
	}

	// prefer cram-md5 over login...
	//
	std::string result = has_cram ? cram : ( has_login ? login : std::string() ) ;

	// ... but only if a secret is defined for it
	//
	if( !result.empty() && Sasl::instance().clientSecrets().id(result).empty() )
	{
		result = std::string() ;
		if( has_cram && has_login )
		{
			result = login ;
			if( Sasl::instance().clientSecrets().id(login).empty() )
				result = std::string() ;
		}
		static bool first = true ;
		if( first ) G_WARNING( "GSmtp::SaslClient: missing \"login\" or \"cram-md5\" entry in secrets file" ) ;
		first = false ;
	}
	return result ;
}

// ===

GSmtp::Sasl * GSmtp::Sasl::m_this = NULL ;

GSmtp::Sasl::Sasl( const std::string & /*app*/ , const G::Path & client_path , const G::Path & server_path )
{
	if( m_this == NULL )
		m_this = this ;

	m_client_secrets <<= new Secrets(client_path) ;
	m_server_secrets <<= new Secrets(server_path) ;
}

GSmtp::Sasl::~Sasl()
{
	if( m_this == this )
		m_this = NULL ;
}

GSmtp::Sasl & GSmtp::Sasl::instance()
{
	if( m_this == NULL )
		throw Error( "no instance" ) ;
	return * m_this ;
}

const GSmtp::Secrets & GSmtp::Sasl::clientSecrets() const
{
	return *m_client_secrets.get() ;
}

const GSmtp::Secrets & GSmtp::Sasl::serverSecrets() const
{
	return *m_server_secrets.get() ;
}

// not implemented...
//void GSmtp::Sasl::check( const std::string & op , int rc ) const {}

