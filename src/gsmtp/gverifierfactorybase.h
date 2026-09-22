//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gverifierfactorybase.h
///

#ifndef G_SMTP_VERIFIER_FACTORY_BASE_H
#define G_SMTP_VERIFIER_FACTORY_BASE_H

#include "gdef.h"
#include "gverifier.h"
#include "geventstate.h"
#include "gstringview.h"
#include <string>
#include <utility>
#include <memory>

namespace GSmtp
{
	class VerifierFactoryBase ;
}

//| \class GSmtp::VerifierFactoryBase
/// A factory interface for addresss verifiers.
///
class GSmtp::VerifierFactoryBase
{
public:
	struct Spec /// Verifier specification tuple for GSmtp::VerifierFactoryBase::newVerifier().
	{
		Spec() ;
		Spec( std::string_view , std::string_view ) ;
		std::string first ; // "exit", "file", "net", empty on error
		std::string second ; // reason on error, or eg. "/bin/a" if "file"
	} ;

	virtual std::unique_ptr<Verifier> newVerifier( GNet::EventState ,
		const Verifier::Config & config , const Spec & spec ) = 0 ;
			///< Returns a Verifier on the heap. Throws if an invalid
			///< or unsupported specification.

	virtual ~VerifierFactoryBase() = default ;
		///< Destructor.
} ;

#endif
