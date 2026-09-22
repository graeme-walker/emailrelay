//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gtest.h
///

#ifndef G_TEST_H
#define G_TEST_H

#include "gdef.h"
#include <string>

namespace G
{
	class Test ;
}

//| \class G::Test
/// A static interface for enabling test features at run-time. Typically does
/// nothing in a release build. Test are enabled by a specification string
/// that is a comma-separated list of test names. The test specification
/// is taken from an environment variable by default, or it can be set
/// programatically.
///
/// Eg:
/// \code
///  for(..)
///  {
///   if( G::Test::enabled("run-loop-extra-slowly") )
///      sleep(1) ;
///   ...
///  }
/// \endcode
///
class G::Test
{
public:
	static void set( const std::string & ) ;
		///< Sets the test specification string.

	static bool enabled() noexcept ;
		///< Returns true if test features are enabled.

	static bool enabled( const char * name ) ;
		///< Returns true if the specified test feature is enabled.

public:
	Test() = delete ;
} ;

#if defined(_DEBUG) || defined(G_TEST_ENABLED)
//
#else
inline bool G::Test::enabled( const char * )
{
	return false ;
}
#endif

#endif
