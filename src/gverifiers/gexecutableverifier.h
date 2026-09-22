//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gexecutableverifier.h
///

#ifndef G_EXECUTABLE_VERIFIER_H
#define G_EXECUTABLE_VERIFIER_H

#include "gdef.h"
#include "gverifier.h"
#include "gtask.h"
#include "gtimer.h"
#include <string>

namespace GVerifiers
{
	class ExecutableVerifier ;
}

//| \class GVerifiers::ExecutableVerifier
/// A Verifier that runs an executable.
///
class GVerifiers::ExecutableVerifier : public GSmtp::Verifier, private GNet::TaskCallback
{
public:
	ExecutableVerifier( GNet::EventState , const GSmtp::Verifier::Config & , const G::Path & ) ;
		///< Constructor.

private: // overrides
	G::Slot::Signal<GSmtp::Verifier::Command,const GSmtp::VerifierStatus&> & doneSignal() override ; // GSmtp::Verifier
	void cancel() override ; // GSmtp::Verifier
	void onTaskDone( int , const std::string & ) override ; // GNet::TaskCallback
	void verify( const GSmtp::Verifier::Request & ) override ; // GSmtp::Verifier

public:
	~ExecutableVerifier() override = default ;
	ExecutableVerifier( const ExecutableVerifier & ) = delete ;
	ExecutableVerifier( ExecutableVerifier && ) = delete ;
	ExecutableVerifier & operator=( const ExecutableVerifier & ) = delete ;
	ExecutableVerifier & operator=( ExecutableVerifier && ) = delete ;

private:
	void onTimeout() ;

private:
	GNet::Timer<ExecutableVerifier> m_timer ;
	GSmtp::Verifier::Command m_command {GSmtp::Verifier::Command::VRFY} ;
	GSmtp::Verifier::Config m_config ;
	G::Path m_path ;
	G::Slot::Signal<GSmtp::Verifier::Command,const GSmtp::VerifierStatus&> m_done_signal ;
	std::string m_to_address ;
	GNet::Task m_task ;
} ;

#endif
