//
// Copyright (C) 2001 Graeme Walker <graeme_walker@users.sourceforge.net>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
// ===
//
// commandline.cpp
//

#ifndef G_MAIN_COMMAND_LINE_H
#define G_MAIN_COMMAND_LINE_H

#include "gdef.h"
#include "gsmtp.h"
#include "garg.h"
#include "configuration.h"
#include "ggetopt.h"
#include <string>
#include <iostream>

namespace Main
{
	class CommandLine ;
} ;

// Class: Main::CommandLine
// Description: A class which deals with the command-line interface
// to the process (both input and output).
//
class Main::CommandLine
{
public:
	CommandLine( const G::Arg & arg , const std::string & version ) ;
		// Constructor.

	Configuration cfg() const ;
		// Returns a Configuration object.

	bool contains( const std::string & switch_ ) const ;
		// Returns true if the command line contained the give switch.

	std::string value( const std::string & switch_ ) const ;
		// Returns the given switch's value.

	unsigned int argc() const ;
		// Returns the number of non-switch arguments on the command line.

	bool hasUsageErrors() const ;
		// Returns true if the command line has usage errors (eg. invalid switch).

	bool hasSemanticError() const ;
		// Returns true if the command line has logical errors (eg. conflicting switches).

	void showUsage( bool error_stream = false ) const ;
		// Writes usage info.

	void showHelp( bool error_stream = false ) const ;
		// Writes help text.

	void showUsageErrors( bool error_stream = true ) const ;
		// Writes the usage errors.

	void showSemanticError( bool error_stream = true ) const ;
		// Writes the logic errors.

	void showArgcError( bool error_stream = true ) const ;
		// Writes a too-many-arguments error message.

	void showNoop( bool error_stream = false ) const ;
		// Writes a nothing-to-do message.

	void showVersion( bool error_stream = false ) const ;
		// Writes the version number.

	void showBanner( bool error_stream = false ) const ;
		// Writes a startup banner.

	void showCopyright( bool error_stream = false ) const ;
		// Writes a copyright message.

	static std::string warranty( const std::string & eol = std::string("\n") ) ;
		// Returns the warranty text.

	static std::string copyright() ;
		// Returns the copyright text.

private:
	void showWarranty( bool error_stream ) const ;
	void showShortHelp( bool error_stream ) const ;
	std::string semanticError() const ;
	static std::string switchSpec() ;
	static std::string osSwitchSpec() ; // o/s-specific
	unsigned int ttyColumns() const ; // o/s-specific
	void showExtraHelp( bool error_stream ) const ;

private:
	std::string m_version ;
	G::Arg m_arg ;
	G::GetOpt m_getopt ;

private:
	class Show
	{
		public: explicit Show( bool e ) ;
		public: std::ostream & s() ;
		public: ~Show() ;
		private: static Show * m_this ;
		private: std::stringstream m_ss ;
		private: bool m_e ;
	} ;
} ;

#endif
