//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file ggetopt.cpp
///

#include "gdef.h"
#include "ggetopt.h"
#include "goptionparser.h"
#include "goptionreader.h"
#include "gstr.h"
#include "gassert.h"
#include "glog.h"
#include <fstream>
#include <algorithm>

G::GetOpt::GetOpt( const Arg & args_in , const std::string & spec , std::size_t ignore_non_options ) :
	m_spec(spec) ,
	m_args(args_in)
{
	parseArgs( ignore_non_options ) ;
}

G::GetOpt::GetOpt( const Arg & args_in , const Options & spec , std::size_t ignore_non_options ) :
	m_spec(spec) ,
	m_args(args_in)
{
	parseArgs( ignore_non_options ) ;
}

G::GetOpt::GetOpt( const StringArray & args_in , const std::string & spec , std::size_t ignore_non_options ) :
	m_spec(spec) ,
	m_args(args_in)
{
	parseArgs( ignore_non_options ) ;
}

G::GetOpt::GetOpt( const StringArray & args_in , const Options & spec , std::size_t ignore_non_options ) :
	m_spec(spec) ,
	m_args(args_in)
{
	parseArgs( ignore_non_options ) ;
}

void G::GetOpt::reload( const StringArray & args_in , std::size_t ignore_non_options )
{
	m_map.clear() ;
	m_errors.clear() ;
	m_args = Arg( args_in ) ;
	parseArgs( ignore_non_options ) ;
}

void G::GetOpt::parseArgs( std::size_t ignore_non_options )
{
	StringArray new_args = OptionParser::parse( m_args.array() , m_spec , m_map , &m_errors , 1U , ignore_non_options ) ;
	new_args.insert( new_args.begin() , m_args.v(0U) ) ;
	m_args = Arg( new_args ) ;
}

bool G::GetOpt::addOptionsFromFile( std::size_t n , const StringArray & blocklist )
{
	if( n < m_args.c() )
	{
		G::Path path = m_args.v( n ) ;
		if( std::find( blocklist.begin() , blocklist.end() , path.extension() ) != blocklist.end() )
			return false ;
		m_args.removeAt( n ) ;
		addOptionsFromFile( path ) ;
	}
	return true ;
}

void G::GetOpt::addOptionsFromFile( std::size_t n , const std::string & varkey , const std::string & varvalue )
{
	if( n < m_args.c() )
	{
		std::string filename = m_args.v( n ) ;
		m_args.removeAt( n ) ;

		if( !filename.empty() )
		{
			if( !varkey.empty() && !varvalue.empty() && filename.find(varkey) == 0 )
				G::Str::replace( filename , varkey , varvalue ) ;
			addOptionsFromFile( filename ) ;
		}
	}
}

G::StringArray G::GetOpt::readOptionsFromFile( const Path & filename )
{
	return OptionReader::read( filename ) ;
}

void G::GetOpt::addOptionsFromFile( const Path & filename )
{
	OptionParser::parse( readOptionsFromFile(filename) , m_spec , m_map , &m_errors , 0U ) ;
}

const std::vector<G::Option> & G::GetOpt::options() const
{
	return m_spec.list() ;
}

const G::OptionMap & G::GetOpt::map() const
{
	return m_map ;
}

G::StringArray G::GetOpt::errorList() const
{
	return m_errors ;
}

bool G::GetOpt::contains( char c ) const
{
	return m_map.contains( m_spec.lookup(c) ) ;
}

bool G::GetOpt::contains( std::string_view name ) const
{
	return m_map.contains( name ) ;
}

std::size_t G::GetOpt::count( std::string_view name ) const
{
	return m_map.count( name ) ;
}

std::string G::GetOpt::value( char c , std::string_view default_ ) const
{
	G_ASSERT( contains(c) ) ;
	return value( m_spec.lookup(c) , default_ ) ;
}

std::string G::GetOpt::value( std::string_view name , std::string_view default_ ) const
{
	return m_map.value( name , default_ ) ;
}

G::Arg G::GetOpt::args() const
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

