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
// gsasl.h
//

#ifndef G_SASL_H
#define G_SASL_H

#include "gdef.h"
#include "gsmtp.h"
#include "gsecrets.h"
#include "gexception.h"
#include "gstrings.h"
#include "gpath.h"
#include <map>
#include <memory>

namespace GSmtp
{
	class Sasl ;
	class SaslImp ;
	class SaslClient ;
	class SaslClientImp ;
	class SaslServer ;
	class SaslServerImp ;
}

// Class: GSmtp::Sasl
// Description: A singleton class representing the SASL library.
// The SASL challenge/response concept is described in RFC2222.
// See also: GSmtp::SaslClient, GSmtp::SaslServer, RFC2554, RFC2222
//
class GSmtp::Sasl
{
public:
	G_EXCEPTION( Error , "sasl library error" ) ;

	Sasl( const std::string & application_name ,
		const G::Path & client_secrets , const G::Path & server_secrets ) ;
			// Constructor.

	~Sasl() ;
		// Destructor.

	static Sasl & instance() ;
		// Singleton access.

	const Secrets & clientSecrets() const ;
		// Returns a reference to the client-side secrets object.

	const Secrets & serverSecrets() const ;
		// Returns a reference to the server-side secrets object.

	void check( const std::string & op , int rc , const char * more = NULL ) const ;
		// Used by implementation classes to check function
		// return codes.

	std::string errorString( const std::string & op , int rc , const char * more = NULL ) const ;
		// Used by implementation classes to interpret function
		// return codes.

private:
	Sasl( const Sasl & ) ; // not implemented
	void operator=( const Sasl & ) ; // not implemented
	typedef std::map<int,std::string GLessAllocator(int,std::string) > Map ;
	void insert( int , const std::string & ) ;
	void initMap() ;
	void initClient( const G::Path & ) ;
	void initServer( const std::string & , const G::Path & ) ;

private:
	static Sasl * m_this ;
	Map m_map ;
	std::auto_ptr<Secrets> m_client_secrets ;
	std::auto_ptr<Secrets> m_server_secrets ;
} ;

// Class: GSmtp::SaslServer
// Description: A class for implementing the server-side SASL
// challenge/response concept, as described in RFC2222.
//
// Usage:
/// SaslServer sasl ;
/// if( sasl.init("MD5") )
/// {
///   client.send( sasl.initialChallenge() ) ;
///   for(;;)
///   {
///     std::string reply = client.read() ;
///     bool done = false ;
///     std::string challenge = sasl.apply( reply , done ) ;
///     if( done ) break ;
///     client.send( challenge ) ;
///   }
///   bool ok = sasl.authenticated() ;
/// }
//
// See also: GSmtp::SaslClient, RFC2554, RFC2222
//
class GSmtp::SaslServer
{
public:
	SaslServer() ;
		// Default constructor.

	~SaslServer() ;
		// Destructor.

	bool active() const ;
		// Returns true if the constructor's "secrets" object
		// was valid. See also Secrets::valid().

	bool init( const std::string & mechanism ) ;
		// Initialiser. Returns true if a supported mechanism.
		// May be used more than once.

	bool mustChallenge() const ;
		// Returns true if the mechanism must start with
		// a non-empty server challenge.

	std::string initialChallenge() const ;
		// Returns the initial server challenge. May return
		// an empty string.

	std::string apply( const std::string & response , bool & done ) ;
		// Applies the client response and returns the
		// next challenge.

	bool authenticated() const ;
		// Returns true if authenticated sucessfully.
		// Precondition: apply() returned empty

	std::string id() const ;
		// Returns the authenticated identity. Returns the
		// empty string if not authenticated.

	std::string mechanisms( char sep = ' ' ) const ;
		// Returns a list of supported mechanisms.

private:
	SaslServer( const SaslServer & ) ; // not implemented
	void operator=( const SaslServer & ) ; // not implemented

private:
	SaslServerImp * m_imp ;
} ;

// Class: GSmtp::SaslClient
// Description: A class for implementing the client-side SASL
// challenge/response concept.
// See also: GSmtp::SaslServer, RFC2222, RFC2554.
//
class GSmtp::SaslClient
{
public:
	explicit SaslClient( const std::string & server_name ) ;
		// Constructor.

	~SaslClient() ;
		// Destructor.

	bool active() const ;
		// Returns true if the constructor's secrets object
		// is valid.

	std::string response( const std::string & mechanism , const std::string & challenge ,
		bool & done , bool & error ) const ;
			// Returns a response to the given challenge.

	std::string preferred( const G::Strings & mechanisms ) const ;
		// Returns the name of the preferred mechanism taken from
		// the given set. Returns the empty string if none is
		// supported or if not active().

private:
	SaslClient( const SaslClient & ) ; // not implemented
	void operator=( const SaslClient & ) ; // not implemented

private:
	SaslClientImp * m_imp ;
} ;

#endif
