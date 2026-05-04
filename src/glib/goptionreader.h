//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
// 
// Copyright (c) 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// ===
///
/// \file goptionreader.h
///

#ifndef G_OPTION_READER_H
#define G_OPTION_READER_H

#include "gdef.h"
#include "gstringarray.h"
#include "gexception.h"
#include "ggettext.h"
#include "gpath.h"
#include <vector>

namespace G
{
	class OptionReader ;
}

//| \class G::OptionReader
/// Provides a static function to read options from a config file, or
/// from a config file and/or associated drop-in files. Also supports
/// UAPI semantics of prioritied search directories.
///
/// The configuration structure can be initialised from a specification
/// string like "single", "dropins", "uapi:foo.conf:/etc:/usr/foo".
///
/// \code
/// std::string config_spec = ... ;
/// Path path = ... ;
/// OptionReader::Config config ;
/// config.parse( config_spec , "foo" ) ;
/// if( config.uapi() && !path.empty() )
///   throw UsageError() ;
/// StringArray options ;
/// StringArray errors ;
/// OptionReader::read( options , errors , path , config ) ;
/// \endcode
///
class G::OptionReader
{
public:
	G_EXCEPTION( FileError , tx("error reading configuration file") )
	using PathArray = std::vector<Path> ;
	enum class Mode { single , dropins , uapi } ;
	struct Config
	{
		Config() ;
		bool parse( std::string_view spec , std::string_view name ) ;
		bool uapi() const ;
		Mode mode ;
		std::string filename ; // uapi filename -- read() path empty
		std::vector<Path> dirs ; // uapi search directories
		std::size_t limit ;
		std::string suffix ; // dropins ".d"
		bool debug ; // debug to stderr
	} ;

	static void read( StringArray & options_out , PathArray & files_out ,
		StringArray & errors_out , const Path & config_file , const Config & = {} ) ;
			///< Reads options from a configuration file and/or drop-in
			///< files, adding to the given output array.
			///<
			///< Leading and trailing whitespace on a line are removed.
			///< Comment lines starting with '#' are ignored. Keys and
			///< values are split by whitepsace or by "=". Simple quotes,
			///< with double-quotes at the start and end of a value, are
			///< stripped.
			///<
			///< Output strings are formatted as "--foo=bar", suitable
			///< for G::OptionParser::parse(), without quoting.
			///<
			///< In 'single' Config mode only the specified file is read; in
			///< 'dropins' mode any associated drop-in files are also read;
			///< and in 'uapi' mode config files and drop-in files are
			///< searched for in the configured directories and the
			///< path parameter is ignored and should be empty.
			///<
			///< Any file i/o errors are added to the errors output array.
			///<
			///< Does not do any effective userid switching (see G::Root).

	static void read( StringArray & options_out ,
		const Path & config_file , const Config & = {} ) ;
			///< An overload that throws the first file i/o error.

public:
	OptionReader() = delete ;

private:
	static void read( StringArray & , PathArray * , StringArray * , const Path & , const Config & ) ;
	static void fail( StringArray * , const Path & ) ;
	static bool add( StringArray & , const Path & , std::size_t limit ) ;
	static void findFiles( std::vector<Path> & , const Path & , const Config & ) ;
	static std::vector<Path> findDropins( const Path & , const std::string & ) ;
	static bool seen( const std::vector<Path> & , const Path & ) ;
	static bool exists( const Path & ) ;
} ;

inline
bool G::OptionReader::Config::uapi() const
{
	return mode == Mode::uapi ;
}

#endif
