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
// gsasl_login.cpp
//

#include "gdef.h"
#include "gsmtp.h"
#include "gsasl.h"
#include "gstr.h"
#include "gmemory.h"
#include "gdebug.h"

namespace
{
	const char * prompt_1 = "Username:" ;
	const char * prompt_2 = "Password:" ;
} ;

// ===

// Class: GSmtp::SaslServerImp
// Description: A private pimple-pattern implementation class used by GSmtp::SaslServer.
//
class GSmtp::SaslServerImp
{
public:
	bool m_first ;
	std::string m_mechanism ;
	bool m_authenticated ;
	std::string m_id ;
	SaslServerImp() ;
} ;

GSmtp::SaslServerImp::SaslServerImp() :
	m_first(true) ,
	m_authenticated(false)
{
}

// ===

std::string GSmtp::SaslServer::mechanisms( char c ) const
{
	std::string sep( 1U , c ) ;
	std::string s = std::string("LOGIN") ; // + sep + "CRAM-MD5" + sep + "DIGEST-MD5" ;
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
	m_imp->m_mechanism = mechanism ;
	m_imp->m_authenticated = false ;
	m_imp->m_id = std::string() ;
	m_imp->m_first = true ;

	G_DEBUG( "GSmtp::SaslServer::init: mechanism \"" << m_imp->m_mechanism << "\"" ) ;
	return m_imp->m_mechanism == "LOGIN" ;
}

std::string GSmtp::SaslServer::initialChallenge() const
{
	return prompt_1 ;
}

std::string GSmtp::SaslServer::apply( const std::string & response , bool & done )
{
	done = false ;
	G_DEBUG( "GSmtp::SaslServer::apply: \"" << response << "\"" ) ;
	std::string next_challenge ;
	if( m_imp->m_first )
	{
		m_imp->m_first = false ;
		m_imp->m_id = response ;
		if( !m_imp->m_id.empty() )
			next_challenge = prompt_2 ;
	}
	else
	{
		m_imp->m_first = true ;
		m_imp->m_authenticated =
			!response.empty() &&
			response == Sasl::instance().serverSecrets().secret(m_imp->m_mechanism,m_imp->m_id) ;
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
	if( challenge == prompt_1 )
	{
		rsp = Sasl::instance().clientSecrets().id(mechanism) ;
		error = rsp.empty() ;
		done = false ;
	}
	else if( challenge == prompt_2 )
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
	const std::string login( "LOGIN" ) ;
	bool has_login = false ;
	for( G::Strings::const_iterator p = mechanism_list.begin() ; p != mechanism_list.end() ; ++p )
	{
		std::string mechanism = *p ;
		G::Str::toUpper( mechanism ) ;
		G_DEBUG( "GSmtp::SaslClient::preferred: \"" << mechanism << "\"" ) ;
		if( mechanism == login )
			has_login = true ; // (no break for diagnostic purposes)
	}
	if( has_login && Sasl::instance().clientSecrets().id(login).empty() )
	{
		G_WARNING( "GSmtp::SaslClient: no \"login client\" entry in secrets file" ) ;
	}
	return has_login && !Sasl::instance().clientSecrets().id(login).empty() ? login : std::string() ;
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

