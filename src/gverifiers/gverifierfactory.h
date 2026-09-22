//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gverifierfactory.h
///

#ifndef G_VERIFIER_FACTORY_H
#define G_VERIFIER_FACTORY_H

#include "gdef.h"
#include "gverifierfactorybase.h"
#include "gverifier.h"
#include "geventstate.h"
#include "gstringview.h"
#include "gstringarray.h"
#include <string>
#include <utility>
#include <memory>

namespace GVerifiers
{
	class VerifierFactory ;
}

//| \class GVerifiers::VerifierFactory
/// A VerifierFactory implementation.
///
class GVerifiers::VerifierFactory : public GSmtp::VerifierFactoryBase
{
public:
	VerifierFactory() ;
		///< Constructor.

	static Spec parse( std::string_view spec , const G::Path & base_dir = {} ,
		const G::Path & app_dir = {} , G::StringArray * warnings_p = nullptr ) ;
			///< Parses a verifier specification string like "/usr/bin/foo" or
			///< "net:127.0.0.1:99" or "net:/run/spamd.s", returning the
			///< type and value in a Spec tuple, eg. ("file","/usr/bin/foo")
			///< or ("net","127.0.0.1:99").
			///<
			///< Any relative file paths are made absolute using the given
			///< base directory, if given. (This is normally from
			///< G::Process::cwd() called at startup).
			///<
			///< Any "@app" sub-strings in file paths are substituted with
			///< the given application directory, if given.
			///<
			///< Returns 'first' empty if a fatal parsing error, with the
			///< reason in 'second'.
			///<
			///< Returns warnings by reference for non-fatal errors, such
			///< as missing files.

protected: // overrides
	std::unique_ptr<GSmtp::Verifier> newVerifier( GNet::EventState ,
		const GSmtp::Verifier::Config & config ,
		const GSmtp::VerifierFactoryBase::Spec & spec ) override ;

private:
	static void checkFile( Spec & result , G::StringArray * warnings_p ) ;
	static void fixFile( Spec & result , const G::Path & base_dir , const G::Path & app_dir ) ;
	static void checkNet( Spec & result ) ;
	static void checkRange( Spec & result ) ;
	static void checkExit( Spec & result ) ;
} ;

#endif
