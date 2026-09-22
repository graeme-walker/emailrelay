//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file ginternalverifier.cpp
///

#include "gdef.h"
#include "ginternalverifier.h"
#include "glog.h"

GVerifiers::InternalVerifier::InternalVerifier()
= default;

void GVerifiers::InternalVerifier::verify( const GSmtp::Verifier::Request & request )
{
	// accept all addresses as if remote
	auto status = GSmtp::VerifierStatus::remote( request.address ) ;
	doneSignal().emit( request.command , status ) ;
}

G::Slot::Signal<GSmtp::Verifier::Command,const GSmtp::VerifierStatus&> & GVerifiers::InternalVerifier::doneSignal()
{
	return m_done_signal ;
}

void GVerifiers::InternalVerifier::cancel()
{
}

