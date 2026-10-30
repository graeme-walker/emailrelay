//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file output.h
///

#ifndef G_MAIN_OUTPUT_H
#define G_MAIN_OUTPUT_H

#include "gdef.h"
#include "goptionsusage.h"
#include <string>

namespace Main
{
	class Output ;
}

//| \class Main::Output
/// An abstract interface for generating output on a command-line
/// or a GUI. The appropriate implementation is selected from
/// main() or WinMain().
///
class Main::Output
{
public:
	virtual void output( const std::string & , bool is_error , bool ) = 0 ;
		///< Outputs the given string.

	virtual G::OptionsUsage::Config outputLayout( bool verbose ) const = 0 ;
		///< Returns a layout definition for G::Options.

	virtual bool outputSimple() const = 0 ;
		///< Returns true if the output is just sent to stdout;
		///< returns false for a fancy gui message box.

	virtual ~Output() = default ;
		///< Destructor.
} ;

#endif
