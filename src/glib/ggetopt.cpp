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
// ggetopt.cpp
//	

#include "gdef.h"
#include "glog.h"
#include "gstrings.h"
#include "gstr.h"
#include "ggetopt.h"
#include "gassert.h"
#include "gdebug.h"

static void GetOpt__pushBack( void * out , const std::string & s )
{
	reinterpret_cast<G::GetOpt::StringArray*>(out)->push_back(s) ;
}

G::GetOpt::GetOpt( const Arg & args_in , const std::string & spec ,
	char sep_major , char sep_minor , char escape ) :
		m_args(args_in)
{
	parseSpec( spec , sep_major , sep_minor , escape ) ;
	size_t n = parseArgs( args_in ) ;
	remove( n ) ;
}

void G::GetOpt::parseSpec( const std::string & spec , char sep_major , char sep_minor , char escape )
{
	if( spec.find(sep_minor) == std::string::npos )
		parseOldSpec( spec ) ;
	else
		parseNewSpec( spec , sep_major , sep_minor , escape ) ;
}

void G::GetOpt::parseOldSpec( const std::string & spec )
{
	for( size_t i = 0U ; i < spec.length() ; i++ )
	{
		char c = spec.at(i) ;
		if( c != ':' )
		{
			bool valued = (i+1U) < spec.length() && spec.at(i+1U) == ':' ;
			addSpec( c , valued ) ;
		}
	}
}

void G::GetOpt::parseNewSpec( const std::string & spec , char sep_major ,
	char sep_minor , char escape )
{
	Strings outer ;
	std::string ws_major( 1U , sep_major ) ;
	G::Str::splitIntoFields( spec , outer , ws_major , escape , false ) ;
	for( Strings::iterator p = outer.begin() ; p != outer.end() ; ++p )
	{
		StringArray inner ;
		std::string ws_minor( 1U , sep_minor ) ;
		G::Str::splitIntoFields( *p , inner , ws_minor , escape ) ;
		if( inner.size() != 5U )
			throw InvalidSpecification(std::stringstream() << "\"" << *p << "\" (" << ws_minor << ")") ;
		bool valued = G::Str::toUInt( inner[3U] ) != 0U ;
		addSpec( inner[0U].at(0U) , inner[1U] , inner[2U] , valued , inner[4U] ) ;
	}
}

void G::GetOpt::addSpec( char c , bool valued )
{
	addSpec( c , std::string() , std::string() , valued , std::string() ) ;
}

void G::GetOpt::addSpec( char c , const std::string & name , const std::string & description ,
	bool valued , const std::string & value_description )
{
	if( c == '\0' )
		throw InvalidSpecification() ;

	std::pair<SwitchSpecMap::iterator,bool> rc =
		m_spec_map.insert( std::make_pair(c,SwitchSpec(c,name,description,valued,value_description)) ) ;

	if( ! rc.second )
		throw InvalidSpecification("duplication") ;
}

bool G::GetOpt::valued( const std::string & name ) const
{
	return valued(key(name)) ;
}

bool G::GetOpt::valued( char c ) const
{
	SwitchSpecMap::const_iterator p = m_spec_map.find( c ) ;
	if( p == m_spec_map.end() )
		return false ;
	else
		return (*p).second.valued ;
}

char G::GetOpt::key( const std::string & name ) const
{
	for( SwitchSpecMap::const_iterator p = m_spec_map.begin() ; p != m_spec_map.end() ; ++p )
	{
		if( (*p).second.name == name )
		{
			return (*p).first ;
		}
	}
	G_DEBUG( "G::GetOpt::key: " << name << " not found" ) ;
	return '\0' ;
}

//static
size_t G::GetOpt::wrapDefault()
{
	return 79U ;
}

//static
size_t G::GetOpt::widthLimit( size_t w )
{
	return (w != 0U && w < 50U) ? 50U : w ;
}

void G::GetOpt::showUsage( std::ostream & stream , const std::string & exe , const std::string & args ,
	size_t tab_stop , size_t width ) const
{
	stream
		<< usageSummary(exe,args,width) << std::endl
		<< usageHelp(tab_stop,width) ;
}

std::string G::GetOpt::usageSummary( const std::string & exe , const std::string & args , size_t width ) const
{
	std::string s = std::string("usage: ") + exe + " " + usageSummarySwitches() + args ;
	if( width != 0U )
	{
		return G::Str::wrap( s , "" , "  " , widthLimit(width) ) ;
	}
	else
	{
		return s ;
	}
}

std::string G::GetOpt::usageSummarySwitches() const
{
	return usageSummaryPartOne() + usageSummaryPartTwo() ;
}

std::string G::GetOpt::usageSummaryPartOne() const
{
	// summarise the single-character switches, excluding those which take a value
	std::stringstream ss ;
	bool first = true ;
	for( SwitchSpecMap::const_iterator p = m_spec_map.begin() ; p != m_spec_map.end() ; ++p )
	{
		if( ! (*p).second.valued )
		{
			if( first )
				ss << "[-" ;
			first = false ;
			ss << (*p).first ;
		}
	}

	std::string s = ss.str() ;
	if( s.length() ) s.append( "] " ) ;
	return s ;
}

std::string G::GetOpt::usageSummaryPartTwo() const
{
	std::stringstream ss ;
	const char * sep = "" ;
	for( SwitchSpecMap::const_iterator p = m_spec_map.begin() ; p != m_spec_map.end() ; ++p )
	{
		ss << sep << "[" ;
		if( (*p).second.name.length() )
		{
			ss << "--" << (*p).second.name ;
		}
		else
		{
			ss << "-" << (*p).first ;
		}
		if( (*p).second.valued )
		{
			std::string vd = (*p).second.value_description ;
			if( vd.empty() ) vd = "value" ;
			ss << " <" << vd << ">" ;
		}
		ss << "]" ;
		sep = " " ;
	}
	return ss.str() ;
}

std::string G::GetOpt::usageHelp( size_t tab_stop , size_t width ) const
{
	return usageHelpCore( "  " , tab_stop , widthLimit(width) ) ;
}

std::string G::GetOpt::usageHelpCore( const std::string & prefix , size_t tab_stop , size_t width ) const
{
	std::string result ;
	for( SwitchSpecMap::const_iterator p = m_spec_map.begin() ; p != m_spec_map.end() ; ++p )
	{
		std::string line( prefix ) ;
		line.append( "-" ) ;
		line.append( 1U , (*p).first ) ;

		if( (*p).second.name.length() )
		{
			line.append( ",--" ) ;
			line.append( (*p).second.name ) ;
		}

		if( (*p).second.valued )
		{
			std::string vd = (*p).second.value_description ;
			if( vd.empty() ) vd = "value" ;
			line.append( " <" ) ;
			line.append( vd ) ;
			line.append( ">" ) ;
		}
		line.append( 1U , ' ' ) ;

		if( line.length() < tab_stop )
			line.append( tab_stop-line.length() , ' ' ) ;

		line.append( (*p).second.description ) ;

		if( width )
		{
			std::string indent( tab_stop , ' ' ) ;
			line = G::Str::wrap( line , "" , indent , width ) ;
		}

		result.append( line ) ;
	}
	return result ;
}

size_t G::GetOpt::parseArgs( const Arg & args_in )
{
	size_t i = 1U ;
	for( ; i < args_in.c() ; i++ )
	{
		const std::string & arg = args_in.v(i) ;
		if( arg == "--" )
		{
			i++ ;
			break ;
		}

		if( isSwitchSet(arg) )
		{
			for( size_t n = 1U ; n < arg.length() ; n++ )
				processSwitch( arg.at(n) ) ;
		}
		else if( isOldSwitch(arg) )
		{
			char c = arg.at(1U) ;
			if( valued(c) && (i+1U) >= args_in.c() )
				errorNoValue( c ) ;
			else if( valued(c) )
				processSwitch( c , args_in.v(++i) ) ;
			else
				processSwitch( c ) ;
		}
		else if( isNewSwitch(arg) )
		{
			std::string name = arg.substr( 2U ) ;
			if( valued(name) && (i+1U) >= args_in.c() )
				errorNoValue( name ) ;
			else if( valued(name) )
				processSwitch( name , args_in.v(++i) ) ;
			else
				processSwitch( name ) ;
		}
		else
		{
			break ;
		}
	}
	i-- ;
	G_DEBUG( "G::GetOpt::parseArgs: removing " << i << " switch args" ) ;
	return i ;
}

bool G::GetOpt::isOldSwitch( const std::string & arg ) const
{
	return
		( arg.length() > 1U && arg.at(0U) == '-' ) &&
		! isNewSwitch( arg ) ;
}

bool G::GetOpt::isNewSwitch( const std::string & arg ) const
{
	return arg.length() > 2U && arg.at(0U) == '-' && arg.at(1U) == '-' ;
}

bool G::GetOpt::isSwitchSet( const std::string & arg ) const
{
	return isOldSwitch(arg) && arg.length() > 2U ;
}

void G::GetOpt::errorNoValue( char c )
{
	std::string e("no value supplied for -") ;
	e.append( 1U , c ) ;
	m_errors.push_back( e ) ;
}

void G::GetOpt::errorNoValue( const std::string & name )
{
	std::string e("no value supplied for --") ;
	e.append( name ) ;
	m_errors.push_back( e ) ;
}

void G::GetOpt::errorUnknownSwitch( char c )
{
	std::string e("invalid switch: -") ;
	e.append( 1U , c ) ;
	m_errors.push_back( e ) ;
}

void G::GetOpt::errorUnknownSwitch( const std::string & name )
{
	std::string e("invalid switch: --") ;
	e.append( name ) ;
	m_errors.push_back( e ) ;
}

void G::GetOpt::processSwitch( const std::string & name )
{
	if( !valid(name) )
	{
		errorUnknownSwitch( name ) ;
		return ;
	}

	char c = key(name) ;
	if( valued(c) )
	{
		errorNoValue( name ) ;
		return ;
	}

	m_map.insert( std::make_pair(c,std::make_pair(false,std::string())) ) ;
}

void G::GetOpt::processSwitch( const std::string & name , const std::string & value )
{
	if( !valid(name) )
	{
		errorUnknownSwitch( name ) ;
		return ;
	}

	char c = key(name) ;
	m_map.insert( std::make_pair(c,std::make_pair(true,value)) ) ;
}

void G::GetOpt::processSwitch( char c )
{
	if( !valid(c) )
	{
		errorUnknownSwitch( c ) ;
		return ;
	}

	if( valued(c) )
	{
		errorNoValue( c ) ;
		return ;
	}

	m_map.insert( std::make_pair(c,std::make_pair(false,std::string())) ) ;
}

void G::GetOpt::processSwitch( char c , const std::string & value )
{
	if( !valid(c) )
		errorUnknownSwitch( c ) ;

	m_map.insert( std::make_pair(c,std::make_pair(true,value)) ) ;
}

G::Strings G::GetOpt::errorList() const
{
	return m_errors ;
}

bool G::GetOpt::contains( char c ) const
{
	SwitchMap::const_iterator p = m_map.find( c ) ;
	return p != m_map.end() ;
}

bool G::GetOpt::contains( const std::string & name ) const
{
	char c = key(name) ;
	SwitchMap::const_iterator p = m_map.find( c ) ;
	return p != m_map.end() ;
}

std::string G::GetOpt::value( char c ) const
{
	G_ASSERT( contains(c) ) ;
	SwitchMap::const_iterator p = m_map.find( c ) ;
	Value value_pair = (*p).second ;
	return value_pair.second ;
}

std::string G::GetOpt::value( const std::string & name ) const
{
	return value( key(name) ) ;
}

G::Arg G::GetOpt::args() const
{
	return m_args ;
}

void G::GetOpt::show( std::ostream &stream , std::string prefix ) const
{
	for( SwitchMap::const_iterator p = m_map.begin() ; p != m_map.end() ; ++p )
	{
		char c = (*p).first ;
		Value v = (*p).second ;

		SwitchSpecMap::const_iterator q = m_spec_map.find( c ) ;
		std::string name ;
		if( q != m_spec_map.end() )
			name = (*q).second.name ;

		stream << prefix << "--" << name ;
		if( v.first )
			stream << " = \"" << v.second << "\"" ;
		stream << std::endl ;
	}
}

bool G::GetOpt::hasErrors() const
{
	return m_errors.size() != 0U ;
}

void G::GetOpt::showErrors( std::ostream &stream , std::string prefix_1 ,
	std::string prefix_2 ) const
{
	if( m_errors.size() != 0U )
	{
		for( Strings::const_iterator p = m_errors.begin() ;
			p != m_errors.end() ; ++p )
		{
			stream << prefix_1 << prefix_2 << *p << std::endl ;
		}
	}
}

void G::GetOpt::remove( size_t n )
{
	if( n != 0U )
	{
		m_args.removeAt(1U,n-1U) ;
	}
}

bool G::GetOpt::valid( const std::string & name ) const
{
	return valid( key(name) ) ;
}

bool G::GetOpt::valid( char c ) const
{
	return m_spec_map.find( c ) != m_spec_map.end() ;
}


