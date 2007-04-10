//
// Copyright (C) 2001-2007 Graeme Walker <graeme_walker@users.sourceforge.net>
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
///
/// \file gsecrets.h
///

#ifndef G_SMTP_SECRETS_H
#define G_SMTP_SECRETS_H

#include "gdef.h"
#include "gsmtp.h"
#include "gpath.h"
#include "gexception.h"
#include "gsasl.h"

/// \namespace GSmtp
namespace GSmtp
{
	class Secrets ;
	class SecretsImp ;
}

/// \class GSmtp::Secrets
/// A simple interface to a store of secrets as used in
/// authentication. The default implementation uses a flat file.
///
/// \see GSmtp::SaslClient, GSmtp::SaslServer
///
class GSmtp::Secrets : public GSmtp::SaslClient::Secrets , public GSmtp::SaslServer::Secrets
{
public:
	G_EXCEPTION( OpenError , "cannot read secrets file" ) ;

	explicit Secrets( const std::string & storage_path ,
		const std::string & debug_name ,
		const std::string & server_type = std::string() ) ;
			///< Constructor. In principle the repository 'storage-path'
			///< can be a path to a file, a database connection string,
			///< etc.
			///<
			///< The 'debug-name' is used in log and error messages to
			///< identify the repository.
			///<
			///< The 'server-type' parameter can be used to select
			///< a different set of server-side authentication records
			///< that may be stored in the same repository.
			///<
			///< Throws on error, although an empty path is not
			///< considered an error: see valid().

	virtual ~Secrets() ;
		///< Destructor.

	virtual bool valid() const ;
		///< Returns false if the path was empty.
		///<
		///< Override from Valid virtual base class.

	virtual std::string id( const std::string & mechanism ) const ;
		///< Returns the default id for client-side
		///< authentication.
		///<
		///< Override from SaslClient::Secrets.

	virtual std::string secret( const std::string & mechanism ) const ;
		///< Returns the default secret for client-side
		///< authentication.
		///<
		///< Override from SaslClient::Secrets.

	virtual std::string secret(  const std::string & mechanism , const std::string & id ) const ;
		///< Returns the given user's secret for server-side
		///< authentication. Returns the empty string if not a
		///< valid id.
		///<
		///< Override from SaslServer::Secrets.

	virtual bool contains( const std::string & mechanism ) const ;
		///< Returns true if there is one or more server
		///< secrets using the given mechanism. This can
		///< be used to limit the list of mechanisms
		///< advertised by a server.
		///<
		///< Override from SaslServer::Secrets.

private:
	Secrets( const Secrets & ) ; // not implemented
	void operator=( const Secrets & ) ; // not implemented

private:
	SecretsImp * m_imp ;
} ;

#endif

