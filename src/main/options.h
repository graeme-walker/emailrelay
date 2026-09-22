//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file options.h
///

#ifndef G_MAIN_OPTIONS_H
#define G_MAIN_OPTIONS_H

#include "gdef.h"
#include "goptions.h"
#include <string>
#include <vector>
#include <utility>

namespace Main
{
	class Options ;
}

//| \class Main::Options
/// Provides the emailrelay command-line options specification string.
/// \see G::OptionParser
///
class Main::Options
{
public:
	static G::Options spec() ;
		///< Returns an o/s-specific G::OptionParser specification.

	using Tag = std::pair<unsigned,std::string> ;

	static std::vector<Tag> tags() ;
		///< Returns an ordered list of tags to be matched
		///< against each option's 'main_tag'.

public:
	Options() = delete ;
} ;

#endif
