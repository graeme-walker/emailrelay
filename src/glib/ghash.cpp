//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file ghash.cpp
///

#include "gdef.h"
#include "ghash.h"
#include "gassert.h"
#include <sstream>

std::string G::Hash::xor_( const std::string & s1 , const std::string & s2 )
{
	G_ASSERT( s1.length() == s2.length() ) ;
	std::string::const_iterator p1 = s1.begin() ;
	std::string::const_iterator p2 = s2.begin() ;
	std::string result ;
	result.reserve( s1.length() ) ;
	for( ; p1 != s1.end() ; ++p1 , ++p2 )
	{
		auto c1 = static_cast<unsigned char>(*p1) ;
		auto c2 = static_cast<unsigned char>(*p2) ;
		auto c = static_cast<unsigned char>( c1 ^ c2 ) ;
		result.append( 1U , static_cast<char>(c) ) ;
	}
	return result ;
}

std::string G::Hash::ipad( std::size_t blocksize )
{
	return std::string( blocksize , '\066' ) ; // NOLINT not return {...}
}

std::string G::Hash::opad( std::size_t blocksize )
{
	return std::string( blocksize , '\134' ) ; // NOLINT not return {...}
}

std::string G::Hash::printable( const std::string & input )
{
	std::string result ;
	result.reserve( input.length() * 2U ) ;
	const char * hex = "0123456789abcdef" ;
	const std::size_t n = input.length() ;
	for( std::size_t i = 0U ; i < n ; i++ )
	{
		auto c = static_cast<unsigned char>(input.at(i)) ;
		result.append( 1U , hex[(c>>4U)&0x0F] ) ;
		result.append( 1U , hex[(c>>0U)&0x0F] ) ;
	}
	G_ASSERT( result.size() == (input.size()*2U) ) ;
	return result ;
}

