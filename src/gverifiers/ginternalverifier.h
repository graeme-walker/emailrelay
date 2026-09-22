//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file ginternalverifier.h
///

#ifndef G_INTERNAL_VERIFIER_H
#define G_INTERNAL_VERIFIER_H

#include "gdef.h"
#include "gverifier.h"
#include "grequestclient.h"
#include "gclientptr.h"
#include <string>

namespace GVerifiers
{
	class InternalVerifier ;
}

//| \class GVerifiers::InternalVerifier
/// The standard internal Verifier that accepts all mailbox names.
///
class GVerifiers::InternalVerifier : public GSmtp::Verifier
{
public:
	InternalVerifier() ;
		///< Constructor.

private: // overrides
	G::Slot::Signal<GSmtp::Verifier::Command,const GSmtp::VerifierStatus&> & doneSignal() override ; // GSmtp::Verifier
	void cancel() override ; // Override from GSmtp::Verifier.
	void verify( const GSmtp::Verifier::Request & ) override ; // GSmtp::Verifier

public:
	~InternalVerifier() override = default ;
	InternalVerifier( const InternalVerifier & ) = delete ;
	InternalVerifier( InternalVerifier && ) = delete ;
	InternalVerifier & operator=( const InternalVerifier & ) = delete ;
	InternalVerifier & operator=( InternalVerifier && ) = delete ;

private:
	G::Slot::Signal<GSmtp::Verifier::Command,const GSmtp::VerifierStatus&> m_done_signal ;
} ;

#endif
