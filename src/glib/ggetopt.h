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
/// \file ggetopt.h
///

#ifndef G_GETOPT_H
#define G_GETOPT_H

#include "gdef.h"
#include "goption.h"
#include "goptions.h"
#include "goptionvalue.h"
#include "goptionparser.h"
#include "goptional.h"
#include "garg.h"
#include "gpath.h"
#include "gstringview.h"
#include "gstringarray.h"
#include <string>

namespace G
{
	class GetOpt ;
}

//| \class G::GetOpt
/// A command-line option parser.
///
/// Usage:
/// \code
///  G::Arg arg( argc , argv ) ;
///  G::Options spec( "e!extra!does something! extra!1!something!1" "|" "h!help!shows help!!0!!1" ) ;
///  G::GetOpt opt( arg , spec , G::GetOpt::Config().set_foo() ) ;
///  if( opt.hasErrors() ) { opt.showErrors( std::cerr ) ; exit( 2 ) ; }
///  if( opt.contains("help") ) { G::OptionsUsage usage( opt.spec() ) ;
///    usage.output( {} , std::cout , arg.prefix() , " [<more>]" ) ;
///    exit( 0 ) ; }
///  run( opt.args() , opt.value("extra","default") ) ;
/// \endcode
///
/// This class is a layer over G::Options, G::OptionMap, G::OptionParser
/// etc. The G::OptionsOutput class is kept separate in order to minimise
/// dependencies.
///
class G::GetOpt
{
public:
	using Pair = std::pair<std::string,OptionMap> ;
	using Maps = std::vector<Pair> ;
	struct Config
	{
		bool with_subcmd {false} ; // have first non-option command-line argument as a sub-command
		std::string section_introducer ; // special option name used to introduce a named section
		bool allow_configfile {false} ; // allow last non-option command-line argument to be a config file path
		std::string configfile_modifier_key ; // eg. "config-files" for "--config-files=dropins"
		std::string configfile_basename ; // see OptionReader::Config::parse()
		std::string configfile_varname ; // config file path substitution variable name
		std::string configfile_varvalue ; // config file path substitution variable value
		Config & set_with_subcmd( bool b = true ) noexcept ;
		Config & set_section_introducer( const std::string & ) ;
		Config & set_allow_configfile( bool b = true ) noexcept ;
		Config & set_configfile_modifier_key( const std::string & ) ;
		Config & set_configfile_basename( const std::string & ) ;
		Config & set_configfile_varname( const std::string & ) ;
		Config & set_configfile_varvalue( const std::string & ) ;
	} ;

	GetOpt( const Arg & arg , const Options & spec , const Config & ) ;
		///< Constructor.
		///<
		///< If a 'config.section_introducer' is defined the parsing will 
		///< allow arbitrary prefixes on command-line options, and each
		///< prefix will have its own G::OptionMap, accessed via maps().
		///<
		///< If 'config.allow_configfile' is true then a non-option 
		///< command-line argument is taken as the name of a config file 
		///< to be loaded, and more than one non-option command-line 
		///< argument is an error.
		///<
		///< If 'config.configfile_modifier_key' is given then it is 
		///< the name of an option that can be used to modify how config 
		///< files are processed. The 'config.configfile_basename'
		///< will also be required to support uapi mode. See 
		///< G::OptionReader::Config::parse().

	GetOpt( const Arg & arg , const char * spec ) ;
		///< Constructor overload.

	GetOpt( const Arg & arg , const char * spec , const Config & ) ;
		///< Constructor overload.

	GetOpt( const Arg & arg , const Options & spec ) ;
		///< Constructor overload.

	const Options & spec() const ;
		///< Exposes the options specification.

	const Maps & maps() const ;
		///< Exposes the set of option-value maps.

	const OptionMap & map() const ;
		///< Exposes the first map of option-values. This will include all
		///< options that had no section name.

	const Arg & args() const ;
		///< Exposes the G::Arg command-line, excluding options and excluding
		///< any processed config file.

	const StringArray & errorList() const ;
		///< Exposes the list of errors.

	const std::vector<Path> & files() const ;
		///< Exposes the list of config files.

	bool hasErrors() const ;
		///< Returns true if there are errors.

	void showErrors( std::ostream & stream , const std::string & prefix_1 ,
		const std::string & prefix_2 = std::string(": ") ) const ;
			///< A convenience function which streams out each errorList()
			///< item to the given stream, prefixed with the given
			///< prefix(es). The two prefixes are simply concatenated.

	void showErrors( std::ostream & stream ) const ;
		///< An overload which has a sensible prefix.

	bool contains( char option_letter ) const ;
		///< Returns true if map() contains the option identified by its
		///< short-form letter.

	bool contains( std::string_view option_name ) const ;
		///< Returns true if map() contains the option identified by its
		///< long-form name.

	std::size_t count( std::string_view option_name ) const ;
		///< Returns the repeat count for a map() value.

	std::string value( std::string_view option_name , std::string_view default_ = {} ) const ;
		///< Returns the map() value for the option identified by its long-form
		///< name. If the option is multi-valued then the returned value is a
		///< comma-separated list. If the option-value is 'on' then
		///< Str::positive() is returned; if the option-value is 'off'
		///< then the given default is returned.

	std::string value( char option_letter , std::string_view default_ = {} ) const ;
		///< An overload that returns the map() value of the option identified
		///< by its short-form letter.
		///< Precondition: contains(option_letter)

	std::optional<std::string> optional( std::string_view option_name ) const ;
		///< Returns an optional map() value identified by its long-form name.

private:
	Arg init( const std::string & , const StringArray & , const Config & ) ;

private:
	Options m_spec ;
	Config m_config ;
	Arg m_args ;
	Maps m_maps ;
	StringArray m_errors ;
	std::vector<Path> m_files ;
} ;

// clang-format off
inline G::GetOpt::Config & G::GetOpt::Config::set_with_subcmd( bool b ) noexcept { with_subcmd = b ; return *this ; }
inline G::GetOpt::Config & G::GetOpt::Config::set_section_introducer( const std::string & s ) { section_introducer = s ; return *this ; }
inline G::GetOpt::Config & G::GetOpt::Config::set_allow_configfile( bool b ) noexcept { allow_configfile = b ; return *this ; }
inline G::GetOpt::Config & G::GetOpt::Config::set_configfile_modifier_key( const std::string & s ) { configfile_modifier_key = s ; return *this ; }
inline G::GetOpt::Config & G::GetOpt::Config::set_configfile_basename( const std::string & s ) { configfile_basename = s ; return *this ; }
inline G::GetOpt::Config & G::GetOpt::Config::set_configfile_varname( const std::string & s ) { configfile_varname = s ; return *this ; }
inline G::GetOpt::Config & G::GetOpt::Config::set_configfile_varvalue( const std::string & s ) { configfile_varvalue = s ; return *this ; }
// clang-format on

#endif
