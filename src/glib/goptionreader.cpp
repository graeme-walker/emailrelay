//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file goptionreader.cpp
///

#include "gdef.h"
#include "goptionreader.h"
#include "gstringview.h"
#include "gstringtoken.h"
#include "gfile.h"
#include "gstr.h"
#include "gexception.h"
#include <fstream>

G::StringArray G::OptionReader::read( const Path & filename , std::size_t limit )
{
	StringArray result ;
	add( result , filename , limit ) ;
	return result ;
}

std::size_t G::OptionReader::add( StringArray & out , const Path & filename , std::size_t limit )
{
	std::ifstream f ;
	File::open( f , filename ) ; // (no G::Root)
	if( !f.good() ) throw FileError( filename.str() ) ;
	std::string line ;
	std::size_t n = 0U ;
	while( Str::readLine(f,line) && ( limit == 0U || n < limit ) )
	{
		if( line.find('\0') != std::string::npos )
			throw G::Exception( "invalid character in configuration file" , filename.str() ) ;

		Str::trimRight( line , "\r" ) ;
		std::string_view sv( line ) ;
		StringTokenView t( sv , " =\t" , 3U ) ;
		std::string_view key = t() ;
		if( key.empty() || key.find('#') == 0U )
			continue ;

		std::string_view value = (++t)() ;
		if( !value.empty() )
			value = Str::trimRightView( sv.substr(t.pos()) , " \t" ) ;
		if( value.size() >= 2U && value[0] == '"' && value[value.size()-1U] == '"' )
			value = value.substr( 1U , value.size() - 2U ) ;

		out.push_back( std::string(2U,'-')
			.append(key.data(),key.size())
			.append(value.empty()?0U:1U,'=')
			.append(value.data(),value.size()) ) ;

		n++ ;
	}
	if( limit && n == limit )
		throw G::Exception( "too many lines in configuration file" , filename.str() ) ;
	return n ;
}

