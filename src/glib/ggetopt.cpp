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
/// \file ggetopt.cpp
///

#include "gdef.h"
#include "ggetopt.h"
#include "goptionparser.h"
#include "goptionreader.h"
#include "gstr.h"
#include "gformat.h"
#include "ggettext.h"
#include "gassert.h"
#include "glog.h"
#include <fstream>
#include <algorithm>

G::GetOpt::GetOpt( const Arg & args_in , const Options & spec ) :
	GetOpt(args_in,Options{spec},{})
{
}

G::GetOpt::GetOpt( const Arg & args_in , const char * spec ) :
	GetOpt(args_in,Options{spec},{})
{
}

G::GetOpt::GetOpt( const Arg & args_in , const char * spec , const Config & config ) :
	GetOpt(args_in,Options{spec},config)
{
}

G::GetOpt::GetOpt( const Arg & args_in , const Options & spec , const Config & config ) :
	m_spec(spec) ,
	m_config(config) ,
	m_args(StringArray())
{
	m_args = init( args_in.v(0U) , args_in.array(1U) , config ) ;
}

G::Arg G::GetOpt::init( const std::string & argv0 , const StringArray & args_in , const Config & config )
{
	auto parser_config =
		OptionParser::Config()
			.set_start_position(0U) // array(1) above
			.set_ignore_non_options(config.with_subcmd?1U:0U)
			.set_keep_negatives(false) ; // (new)

	// get the config file path
	StringArray all_args = args_in ;
	if( config.allow_configfile )
	{
		// get the raw path
		auto configfile_pair = OptionParser::file( all_args , m_spec ,
			m_errors , parser_config , config.configfile_modifier_key ) ;
		G::Path configfile_path = configfile_pair.first ;
		const auto & configfile_modifier = configfile_pair.second ;

		// apply variable substitution to get the real path
		if( !configfile_path.empty() && !m_config.configfile_varname.empty() )
			configfile_path.replace( m_config.configfile_varname , m_config.configfile_varvalue ) ;

		// parse Reader::Config
		OptionReader::Config reader_config ;
		if( !reader_config.parse( configfile_modifier , config.configfile_basename ) )
			m_errors.push_back( str(format(txt("invalid \"--%1%\" value")) % config.configfile_modifier_key) ) ;

		// remove the filename from the command-line
		if( !configfile_path.empty() )
			all_args.pop_back() ; // remove the filename

		// extra checks for uapi mode
		if( reader_config.uapi() )
		{
			if( !configfile_path.empty() )
				m_errors.push_back( txt("cannot specify the config file when using uapi") ) ;
			if( config.configfile_basename.empty() )
				m_errors.push_back( txt("internal error: no configured basename when using uapi") ) ;
		}

		// read the file(s)
		if( !configfile_path.empty() || reader_config.uapi() )
			OptionReader::read( all_args , m_files , m_errors , configfile_path , reader_config ) ;
	}

	// parse out the section names
	auto names = OptionParser::names( all_args , m_spec ,
		m_errors , parser_config , config.section_introducer ) ;

	// parse all options
	auto new_args = OptionParser::parse( all_args , m_spec ,
		names , m_maps , m_errors , parser_config ) ;

	// reassemble G::Arg
	new_args.insert( new_args.begin() , argv0 ) ;
	return Arg(new_args) ;
}

const G::Options & G::GetOpt::spec() const
{
	return m_spec ;
}

const G::GetOpt::Maps & G::GetOpt::maps() const
{
	return m_maps ;
}

const G::OptionMap & G::GetOpt::map() const
{
	return m_maps.at(0U).second ;
}

const G::StringArray & G::GetOpt::errorList() const
{
	return m_errors ;
}

const std::vector<G::Path> & G::GetOpt::files() const
{
	return m_files ;
}

bool G::GetOpt::contains( char option_letter ) const
{
	return map().count( m_spec.lookup(option_letter) ) != 0U ;
}

bool G::GetOpt::contains( std::string_view option_name ) const
{
	return map().count( option_name ) != 0U ;
}

std::size_t G::GetOpt::count( std::string_view option_name ) const
{
	return map().count( option_name ) ;
}

std::string G::GetOpt::value( char option_letter , std::string_view default_ ) const
{
	G_ASSERT( contains(option_letter) ) ;
	return value( m_spec.lookup(option_letter) , default_ ) ;
}

std::string G::GetOpt::value( std::string_view option_name , std::string_view default_ ) const
{
	return map().value( option_name , default_ ) ;
}

std::optional<std::string> G::GetOpt::optional( std::string_view option_name ) const
{
	if( map().count(option_name) == 0U )
		return {} ;
	return { value(option_name) } ;
}

const G::Arg & G::GetOpt::args() const
{
	return m_args ;
}

bool G::GetOpt::hasErrors() const
{
	return !m_errors.empty() ;
}

void G::GetOpt::showErrors( std::ostream & stream ) const
{
	showErrors( stream , m_args.prefix() + ": error" ) ;
}

void G::GetOpt::showErrors( std::ostream & stream , const std::string & prefix_1 , const std::string & prefix_2 ) const
{
	for( const auto & error : m_errors )
	{
		stream << prefix_1 << prefix_2 << error << std::endl ;
	}
}

