//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gexecutablecommand.h
///

#ifndef G_EXECUTABLE_COMMAND_H
#define G_EXECUTABLE_COMMAND_H

#include "gdef.h"
#include "gpath.h"
#include "gstringarray.h"
#include "gexception.h"
#include <string>

namespace G
{
	class ExecutableCommand ;
}

//| \class G::ExecutableCommand
/// A structure representing an external program, holding a path
/// and a set of arguments. The constructor takes a complete command-line and splits it up
/// into the executable part and a list of command-line parameters. If the
/// command-line starts with a script then the contructed command-line may
/// be for the appropriate interpreter (depending on the o/s).
///
/// \see G::Path, G::Args
///
class G::ExecutableCommand
{
public:
	G_EXCEPTION( WindowsError , tx("cannot determine the windows directory") )

	explicit ExecutableCommand( const std::string & command_line = {} ) ;
		///< Constructor taking a complete command-line. The
		///< command-line is split up on unescaped-and-unquoted
		///< space characters. Uses G::Arg::parse() in its
		///< implementation.

	ExecutableCommand( const G::Path & exe , const StringArray & args ) ;
		///< Constructor taking the executable and arguments
		///< explicitly.

	Path exe() const ;
		///< Returns the executable.

	StringArray args() const ;
		///< Returns the command-line arguments.

	void add( const std::string & arg ) ;
		///< Adds a command-line argument.

	void insert( const G::StringArray & ) ;
		///< Inserts at the front of the command-line.
		///< The first element becomes the new executable.

	std::string displayString() const ;
		///< Returns a printable representation for logging and diagnostics.

private:
	G::Path m_exe ;
	G::StringArray m_args ;
} ;

#endif
