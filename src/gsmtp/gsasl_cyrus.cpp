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
// gsasl_cyrus.cpp
//
// An incomplete implementation of GSmtp::Sasl using Carnegie Mellon's "cyrus"
// SASL library (http://www.cmu.edu/computing).
//
// This is work-in-progress which will probably never get completed because
// the cyrus documentation is not good enough.
//

#include "gdef.h"
#include "gsmtp.h"
#include "gsasl.h"
#include "gstr.h"
#include "glocal.h"
#include "gmemory.h"
#include "gdebug.h"
#include <utility>

extern "C"
{
#include <sasl.h>
} ;

// ===

// Class: GSmtp::SaslClientImp
// Description: A private pimple-pattern implementation class used by GSmtp::SaslClient.
//
class GSmtp::SaslClientImp
{
public:
	SaslClientImp( const std::string & server_name ) ;
	~SaslClientImp() ;
	bool active() const ;
	std::string response( const std::string & mechanism ,
		const std::string & challenge , bool & done , bool & error ) const ;
	std::string preferred( const G::Strings & mechanism_list ) const ;

private:
	::sasl_conn_t * m_connection_p ;
} ;

// ===

// Class: GSmtp::SaslServerImp
// Description: A private pimple-pattern implementation class used by GSmtp::SaslServer.
//
class GSmtp::SaslServerImp
{
public:
	SaslServerImp() ;
	~SaslServerImp() ;
	bool active() const ;
	bool init( const std::string & mechanism ) ;
	bool mustChallenge() const ;
	std::string initialChallenge() const ;
	std::string apply( const std::string & response , bool & done ) ;
	bool authenticated() const ;
	std::string id() const ;
	std::string mechanisms( char sep = ' ' ) const ;

private:
	std::string step( const std::string & response , bool & done ) const ;

private:
	::sasl_conn_t * m_connection_p ;
	std::string m_mechanism ;
	bool m_first ;
	bool m_done ;
} ;

// ===

GSmtp::SaslServerImp::SaslServerImp() :
	m_first(true) ,
	m_done(false)
{
	std::string fqdn = GNet::Local::fqdn() ;
	int flags = 0 ; // SASL_SECURITY_LAYER
	int rc = ::sasl_server_new( "smtp" , fqdn.c_str() , NULL , NULL , flags , &m_connection_p ) ;
	Sasl::instance().check( "sasl_server_new" , rc ) ;
}

GSmtp::SaslServerImp::~SaslServerImp()
{
	::sasl_dispose( &m_connection_p ) ;
}

bool GSmtp::SaslServerImp::mustChallenge() const
{
	return false ; // <= can have an initial response
}

bool GSmtp::SaslServerImp::active() const
{
	return Sasl::instance().serverSecrets().valid() ;
}

bool GSmtp::SaslServerImp::authenticated() const
{
	return m_done ;
}

std::string GSmtp::SaslServerImp::id() const
{
	return "" ; // for now
}

std::string GSmtp::SaslServerImp::mechanisms( char sep ) const
{
	char * list = NULL ;
	unsigned int listlen = 0U ;
	unsigned int count = 0U ;
	int rc = ::sasl_listmech( m_connection_p , NULL , "" , "," , "" , &list , &listlen , &count ) ;
	Sasl::instance().check( "sasl_listmech" , rc ) ;

	std::string result ;
	if( list != NULL && listlen != 0 )
	{
		G::Strings word_list ;
		G::Str::splitIntoFields( std::string(list,listlen) , word_list , "," ) ;
		for( G::Strings::iterator p = word_list.begin() ; p != word_list.end() ; ++p )
		{
			if( result.length() )
				result.append( 1U , sep ) ;
			result.append( *p ) ;
		}
	}
	return result ;
}

bool GSmtp::SaslServerImp::init( const std::string & mechanism )
{
	m_mechanism = mechanism ;
	m_first = true ;
	m_done = false ;
	return true ;
}

std::string GSmtp::SaslServerImp::step( const std::string & response , bool & done ) const
{
	const char * error = NULL ;
	char * out = NULL ;
	unsigned int outlen = 0U ;
	const char * response_data = response.length() ? response.data() : NULL ;
	int rc = 0 ;
	const char * op = m_first ? "sasl_server_start" : "sasl_server_step" ;
	if( m_first )
	{
		const_cast<SaslServerImp*>(this)->m_first = false ; // mutable m_first
		rc = ::sasl_server_start( m_connection_p , m_mechanism.c_str() ,
			response_data , response.length() ,
			&out , &outlen , &error ) ;
	}
	else
	{
		rc = ::sasl_server_step( m_connection_p ,
			response_data , response.length() ,
			&out , &outlen , &error ) ;
	}
	Sasl::instance().check( op , rc , error ) ;
	G_ASSERT( rc == SASL_OK || rc == SASL_CONTINUE ) ;
	done = rc == SASL_OK ;

	std::string result ;
	if( !done && out != NULL && outlen != 0U )
		result = std::string( out , outlen ) ;
	return result ;
}

std::string GSmtp::SaslServerImp::apply( const std::string & response , bool & done )
{
	std::string challenge = step( response , done ) ;
	m_done = done ;
	return challenge ;
}

std::string GSmtp::SaslServerImp::initialChallenge() const
{
	bool done = false ;
	std::string challenge = step( "" , done ) ;
	if( done )
		throw Sasl::Error( "no initial challenge" ) ;
	return challenge ;
}

// ===

GSmtp::SaslClientImp::SaslClientImp( const std::string & server_name ) :
	m_connection_p(NULL)
{
	int flags = 0 ;
	int rc = ::sasl_client_new( "smtp" , server_name.c_str() , NULL , flags , &m_connection_p ) ;
	Sasl::instance().check( "sasl_client_new" , rc ) ;
}

GSmtp::SaslClientImp::~SaslClientImp()
{
	::sasl_dispose( &m_connection_p ) ;
}

bool GSmtp::SaslClientImp::active() const
{
	return Sasl::instance().clientSecrets().valid() ;
}

std::string GSmtp::SaslClientImp::preferred( const G::Strings & mechanism_list ) const
{
	std::string list ;
	const char * sep = "" ;
	for( G::Strings::const_iterator mp = mechanism_list.begin() ; mp != mechanism_list.end() ; ++mp , sep = " " )
	{
		list = std::string(sep) + (*mp) ;
	}

	int rc ;
	::sasl_interact_t * interaction_p = NULL ;
	char * out = NULL ;
	unsigned int outlen = 0U ;
	const char * chosen = NULL ;
	rc = ::sasl_client_start( m_connection_p , list.c_str() , NULL , &interaction_p , &out , &outlen , &chosen ) ;
	Sasl::instance().check( "sasl_client_start" , rc ) ;

	std::string result ;
	if( chosen != NULL )
		result = std::string(chosen) ;
	return result ;
}

std::string GSmtp::SaslClientImp::response( const std::string & mechanism ,
	const std::string & challenge , bool & done , bool & error ) const
{
	::sasl_interact_t * interaction_p = NULL ;
	char * out = NULL ;
	unsigned int outlen = 0U ;
	int rc = ::sasl_client_step( m_connection_p , challenge.data() , challenge.length() , &interaction_p ,
		&out , &outlen ) ;
	done = rc == SASL_OK ;
	error = rc != SASL_OK && rc != SASL_CONTINUE ;
	if( error )
		G_WARNING( "GSmtp::SaslClient: authentication error: " <<
			Sasl::instance().errorString("sasl_client_step",rc) ) ;

	std::string result ;
	if( !error && !done && out != NULL && outlen != 0U )
		result = std::string( out , outlen ) ;
	return result ;
}

// ===

GSmtp::Sasl * GSmtp::Sasl::m_this = NULL ;

GSmtp::Sasl::Sasl( const std::string & app_name , const G::Path & client_path , const G::Path & server_path )
{
	if( m_this == NULL )
	{
		m_this = this ;

		initMap() ;
		initClient( client_path ) ;
		initServer( app_name , server_path ) ;
	}
}

void GSmtp::Sasl::initClient( const G::Path & path )
{
	m_client_secrets <<= new Secrets(path) ;
	if( m_client_secrets->valid() )
	{
		static sasl_callback_t callbacks[] =
		{
			{ SASL_CB_GETREALM , NULL , NULL } ,
			{ SASL_CB_USER , NULL , NULL } ,
			{ SASL_CB_AUTHNAME , NULL , NULL } ,
			{ SASL_CB_PASS , NULL , NULL } ,
			{ SASL_CB_LIST_END , NULL , NULL }
		} ;

		int rc = ::sasl_client_init( callbacks ) ;
		check( "sasl_client_init" , rc ) ;
	}
}

void GSmtp::Sasl::initServer( const std::string & app_name , const G::Path & path )
{
	m_server_secrets <<= new Secrets(path) ;
	if( m_server_secrets->valid() )
	{
		static sasl_callback_t callbacks[] =
		{
			{ SASL_CB_GETREALM , NULL , NULL } ,
			{ SASL_CB_USER , NULL , NULL } ,
			{ SASL_CB_AUTHNAME , NULL , NULL } ,
			{ SASL_CB_PASS , NULL , NULL } ,
			{ SASL_CB_LIST_END , NULL , NULL }
		} ;

		int rc = ::sasl_server_init( callbacks , app_name.c_str() ) ;
		check( "sasl_server_init" , rc ) ;
	}
}

GSmtp::Sasl::~Sasl()
{
	if( m_this == this )
	{
		m_this = NULL ;
		::sasl_done() ;
	}
}

GSmtp::Sasl & GSmtp::Sasl::instance()
{
	if( m_this == NULL )
		throw Error( "no instance" ) ;
	return * m_this ;
}

void GSmtp::Sasl::check( const std::string & op , int rc , const char * more_p ) const
{
	if( rc != SASL_OK && rc != SASL_CONTINUE )
	{
		throw Error( errorString(op,rc,more_p) ) ;
	}
}

std::string GSmtp::Sasl::errorString( const std::string & op , int rc , const char * more_p ) const
{
	std::string text = "unknown error" ;
	Map::const_iterator map_p = m_map.find( rc ) ;
	if( map_p != m_map.end() )
		text = (*map_p).second ;

	if( more_p != NULL )
	{
		text.append( ": " ) ;
		text.append( std::string(more_p) ) ;
	}

	return op + ": " + text ;
}

void GSmtp::Sasl::insert( int first , const std::string & second )
{
	m_map.insert( std::make_pair(first,second) ) ;
}

const GSmtp::Secrets & GSmtp::Sasl::clientSecrets() const
{
	G_ASSERT( m_client_secrets.get() != NULL ) ;
	return *m_client_secrets.get() ;
}

const GSmtp::Secrets & GSmtp::Sasl::serverSecrets() const
{
	G_ASSERT( m_server_secrets.get() != NULL ) ;
	return *m_server_secrets.get() ;
}

void GSmtp::Sasl::initMap()
{
	insert( SASL_CONTINUE , "another step is needed in authentication" ) ;
	insert( SASL_OK , "successful result" ) ;
	insert( SASL_FAIL , "generic failure" ) ;
	insert( SASL_NOMEM , "memory shortage failure" ) ;
	insert( SASL_BUFOVER , "overflowed buffer" ) ;
	insert( SASL_NOMECH , "mechanism not supported" ) ;
	insert( SASL_BADPROT , "bad protocol / cancel" ) ;
	insert( SASL_NOTDONE , "cant request info until later in exchange" ) ;
	insert( SASL_BADPARAM , "invalid parameter supplied" ) ;
	insert( SASL_TRYAGAIN , "transient failure (e.g., weak key)" ) ;
	insert( SASL_BADMAC , "integrity check failed" ) ;
	insert( SASL_INTERACT , "needs user interaction" ) ;
	insert( SASL_BADSERV , "server failed mutual authentication step" ) ;
	insert( SASL_WRONGMECH , "mechanism doesnt support requested feature" ) ;
	insert( SASL_NEWSECRET , "new secret needed" ) ;
	insert( SASL_BADAUTH , "authentication failure" ) ;
	insert( SASL_NOAUTHZ , "authorization failure" ) ;
	insert( SASL_TOOWEAK , "mechanism too weak for this user" ) ;
	insert( SASL_ENCRYPT , "encryption needed to use mechanism" ) ;
	insert( SASL_TRANS , "One time use of a plaintext password will enable requested mechanism for user" ) ;
	insert( SASL_EXPIRED , "passphrase expired, has to be reset" ) ;
	insert( SASL_DISABLED , "account disabled" ) ;
	insert( SASL_NOUSER , "user not found" ) ;
	insert( SASL_PWLOCK , "password locked" ) ;
	insert( SASL_NOCHANGE , "requested change was not needed" ) ;
	insert( SASL_BADVERS , "version mismatch with plug-in" ) ;
	insert( SASL_NOPATH , "path not set" ) ;
}

// ===

GSmtp::SaslClient::SaslClient( const std::string & server_name ) :
	m_imp( new SaslClientImp(server_name) )
{
}

GSmtp::SaslClient::~SaslClient()
{
	delete m_imp ;
}

bool GSmtp::SaslClient::active() const
{
	return m_imp->active() ;
}

std::string GSmtp::SaslClient::response( const std::string & mechanism ,
	const std::string & challenge , bool & done , bool & error ) const
{
	return m_imp->response( mechanism , challenge , done , error ) ;
}

std::string GSmtp::SaslClient::preferred( const G::Strings & mechanism_list ) const
{
	return m_imp->preferred( mechanism_list ) ;
}

// ===

GSmtp::SaslServer::SaslServer() :
	m_imp(new SaslServerImp)
{
}

GSmtp::SaslServer::~SaslServer()
{
	delete m_imp ;
}

std::string GSmtp::SaslServer::mechanisms( char c ) const
{
	return m_imp->mechanisms() ;
}

bool GSmtp::SaslServer::active() const
{
	return m_imp->active() ;
}

bool GSmtp::SaslServer::mustChallenge() const
{
	m_imp->mustChallenge() ;
}

bool GSmtp::SaslServer::init( const std::string & mechanism )
{
	m_imp->init( mechanism ) ;
}

std::string GSmtp::SaslServer::initialChallenge() const
{
	return m_imp->initialChallenge() ;
}

std::string GSmtp::SaslServer::apply( const std::string & response , bool & done )
{
	return m_imp->apply( response , done ) ;
}

bool GSmtp::SaslServer::authenticated() const
{
	return m_imp->authenticated() ;
}

std::string GSmtp::SaslServer::id() const
{
	return m_imp->id() ;
}

