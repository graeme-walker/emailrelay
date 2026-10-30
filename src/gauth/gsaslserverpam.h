//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gsaslserverpam.h
///

#ifndef G_SASL_SERVER_PAM_H
#define G_SASL_SERVER_PAM_H

#include "gdef.h"
#include "gsecrets.h"
#include "gsaslserver.h"
#include "gexception.h"
#include "gaddress.h"
#include "gpath.h"
#include <memory>

namespace GAuth
{
	class SaslServerPamImp ;
	class SaslServerPam ;
}

//| \class GAuth::SaslServerPam
/// An implementation of the SaslServer interface using PAM as the
/// authentication mechanism.
///
/// This class tries to match up the PAM interface with the SASL server
/// interface. The match is not perfect; only single-challenge PAM
/// mechanisms are supported, the PAM delay feature is not implemented,
/// and PAM sessions are not part of the SASL interface.
///
class GAuth::SaslServerPam : public SaslServer
{
public:
	explicit SaslServerPam( bool with_apop ) ;
		///< Constructor.

public:
	~SaslServerPam() override ;
	SaslServerPam( const SaslServerPam & ) = delete ;
	SaslServerPam( SaslServerPam && ) = delete ;
	SaslServerPam & operator=( const SaslServerPam & ) = delete ;
	SaslServerPam & operator=( SaslServerPam && ) = delete ;

private: // overrides
	G::StringArray mechanisms( bool ) const override ; // GAuth::SaslServer
	void reset() override ; // GAuth::SaslServer
	bool init( bool , const std::string & mechanism ) override ; // GAuth::SaslServer
	std::string mechanism() const override ; // GAuth::SaslServer
	std::string preferredMechanism( bool ) const override ; // GAuth::SaslServer
	bool mustChallenge() const override ; // GAuth::SaslServer
	std::string initialChallenge() const override ; // GAuth::SaslServer
	std::string apply( const std::string & response , bool & done ) override ; // GAuth::SaslServer
	bool authenticated() const override ; // GAuth::SaslServer
	std::string id() const override ; // GAuth::SaslServer
	bool trusted( const G::StringArray & , const std::string & ) const override ; // GAuth::SaslServer

private:
	std::unique_ptr<SaslServerPamImp> m_imp ;
} ;

#endif
