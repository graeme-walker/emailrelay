//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gnetworkverifier.h
///

#ifndef G_NETWORK_VERIFIER_H
#define G_NETWORK_VERIFIER_H

#include "gdef.h"
#include "gverifier.h"
#include "grequestclient.h"
#include "gclientptr.h"
#include <string>

namespace GVerifiers
{
	class NetworkVerifier ;
}

//| \class GVerifiers::NetworkVerifier
/// A Verifier that talks to a remote address verifier over the network.
///
class GVerifiers::NetworkVerifier : public GSmtp::Verifier , private GNet::ExceptionHandler
{
public:
	NetworkVerifier( GNet::EventState , const GSmtp::Verifier::Config & config ,
		const std::string & server ) ;
			///< Constructor.

	~NetworkVerifier() override ;
		///< Destructor.

private: // overrides
	void verify( const GSmtp::Verifier::Request & ) override ; // GSmtp::Verifier
	G::Slot::Signal<GSmtp::Verifier::Command,const GSmtp::VerifierStatus&> & doneSignal() override ; // GSmtp::Verifier
	void cancel() override ; // GSmtp::Verifier
	void onException( GNet::ExceptionSource * , std::exception & , bool ) override ; // GNet::ExceptionHandler

public:
	NetworkVerifier( const NetworkVerifier & ) = delete ;
	NetworkVerifier( NetworkVerifier && ) = delete ;
	NetworkVerifier & operator=( const NetworkVerifier & ) = delete ;
	NetworkVerifier & operator=( NetworkVerifier && ) = delete ;

private:
	void clientEvent( const std::string & s1 , const std::string & s2 , const std::string & ) ;

private:
	GNet::EventState m_es ;
	G::Slot::Signal<GSmtp::Verifier::Command,const GSmtp::VerifierStatus&> m_done_signal ;
	GSmtp::Verifier::Config m_config ;
	GNet::Location m_location ;
	G::TimeInterval m_connection_timeout ;
	G::TimeInterval m_response_timeout ;
	GNet::ClientPtr<GSmtp::RequestClient> m_client_ptr ;
	std::string m_to_address ;
	GSmtp::Verifier::Command m_command {GSmtp::Verifier::Command::VRFY} ;
} ;

#endif
