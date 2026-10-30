//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file submission.h
///

#ifndef G_MAIN_SUBMISSION_H
#define G_MAIN_SUBMISSION_H

#include "gdef.h"
#include "garg.h"

namespace Main
{
	class Submission ;
}

//| \class Main::Submission
/// Does simple message submission from the command-line.
///
class Main::Submission
{
public:
	static bool enabled( const G::Arg & ) ;
		///< Returns true if the submit functionality is enabled by
		///< the build and argv[0].

	static int submit( const G::Arg & ) ;
		///< Does message submission. Returns an exit code.
} ;

#endif
