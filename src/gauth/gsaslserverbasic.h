//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gsaslserverbasic.h
///

#ifndef G_SASL_SERVER_BASIC_H
#define G_SASL_SERVER_BASIC_H

#include "gdef.h"
#include "gsecrets.h"
#include "gsaslserver.h"
#include "gexception.h"
#include "gaddress.h"
#include "gstringarray.h"
#include "goptional.h"
#include "gpath.h"
#include <memory>
#include <utility>

namespace GAuth
{
	class SaslServerBasicImp ;
	class SaslServerBasic ;
}

//| \class GAuth::SaslServerBasic
/// An implementation of the SaslServer interface that does not use PAM.
/// \see GAuth::SaslServerPam
///
class GAuth::SaslServerBasic : public SaslServer
{
public:
	explicit SaslServerBasic( const SaslServerSecrets & , bool allow_pop ,
		const std::string & config , const std::string & challenge_domain ) ;
			///< Constructor. The 'config' parameters can be used to reduce the
			///< set of available authentication mechanisms.

public:
	~SaslServerBasic() override ;
	SaslServerBasic( const SaslServerBasic & ) = delete ;
	SaslServerBasic( SaslServerBasic && ) = delete ;
	SaslServerBasic & operator=( const SaslServerBasic & ) = delete ;
	SaslServerBasic & operator=( SaslServerBasic && ) = delete ;

private: // overrides
	void reset() override ; // Override from GAuth::SaslServer.
	G::StringArray mechanisms( bool ) const override ; // Override from GAuth::SaslServer.
	bool init( bool , const std::string & mechanism ) override ; // Override from GAuth::SaslServer.
	std::string mechanism() const override ; // Override from GAuth::SaslServer.
	std::string preferredMechanism( bool ) const override ; // Override from GAuth::SaslServer.
	bool mustChallenge() const override ; // Override from GAuth::SaslServer.
	std::string initialChallenge() const override ; // Override from GAuth::SaslServer.
	std::string apply( const std::string & response , bool & done ) override ; // Override from GAuth::SaslServer.
	bool authenticated() const override ; // Override from GAuth::SaslServer.
	std::string id() const override ; // Override from GAuth::SaslServer.
	bool trusted( const G::StringArray & , const std::string & ) const override ; // Override from GAuth::SaslServer.

private:
	std::unique_ptr<SaslServerBasicImp> m_imp ;
} ;

#endif
