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
/// \file gstringvalue.h
///

#ifndef G_STRING_VALUE_H
#define G_STRING_VALUE_H

#include "gdef.h"
#include "gstringview.h"
#include <type_traits>
#include <limits>

namespace G
{
	namespace StringValue
	{
		template <typename T> static T parse( std::string_view , bool & overflow , bool & invalid ,
			T scale = 1 , unsigned int base = 10U , T start = 0 ) noexcept ;
				///< Parses an unsigned numeric string with overflow checking.
				///< The template parameter type can be signed but the string
				///< contents must be unsigned. The base can be between two and
				///< sixteen. Returns zero with 'invalid' set if the string is
				///< empty or not numeric. Returns the type's maximum value
				///< with 'overflow' set if too big.

		namespace detail
		{
			using signed_tag = std::integral_constant<bool,true> ; // std::true_type
			using unsigned_tag = std::integral_constant<bool,false> ; // std::false_type
			template <typename T> struct signage
			{
				using tag = std::integral_constant<bool,std::is_signed<T>::value> ;
			} ;
			template <typename T> static T parse( signed_tag , const char * &p , const char * end ,
					bool & overflow , T scale , unsigned int base , T start ) noexcept ;
			template <typename T> static T parse( unsigned_tag , const char * &p , const char * end ,
					bool & overflow , T scale , unsigned int base , T start ) noexcept ;
			template <typename T> static bool checkedMultiply( T & a , T b ) noexcept ;
			template <typename T> static bool checkedAdd( T & a , T b ) noexcept ;
		}
	}
} ;

template <typename T>
T G::StringValue::parse( std::string_view s , bool & overflow , bool & invalid , T scale , unsigned int base , T start ) noexcept
{
	static_assert( std::is_integral<T>::value , "" ) ;
	overflow = false ;
	invalid = s.empty() || base < 2U || base > 16U ;
	if( invalid )
		return 0UL ;
	const char * p = s.data() ;
	const char * end = s.data() + s.size() ;
	T result = detail::parse<T>( typename detail::signage<T>::tag() , p , end , overflow , scale , base , start ) ;
	if( p != end )
		invalid = true ;
	return result ;
}

template <typename T>
T G::StringValue::detail::parse( signed_tag , const char * &p , const char * end , bool & overflow , T scale , unsigned int base , T start ) noexcept
{
	// do the work using an unsigned type to simplify overflow checking
	static_assert( std::is_signed<T>::value , "" ) ;
	using U = typename std::make_unsigned<T>::type ;
	U uresult = parse( unsigned_tag() , p , end , overflow , static_cast<U>(scale) , base , static_cast<U>(start) ) ;
	if( uresult > static_cast<U>(std::numeric_limits<T>::max()) )
		overflow = true ;
	return overflow ? std::numeric_limits<T>::max() : static_cast<T>(uresult) ;
}

template <typename T>
T G::StringValue::detail::parse( unsigned_tag , const char * &p , const char * end , bool & overflow , T scale , unsigned int base , T start ) noexcept
{
	static_assert( std::is_unsigned<T>::value , "" ) ;
	T result = start ;
	for( bool first = true ; p != end ; p++ , first = false )
	{
		T n = 0 ;
		if( *p == '0' ) n = 0 ;
		else if( *p == '1' ) n = 1 ;
		else if( *p == '2' ) n = 2 ;
		else if( *p == '3' ) n = 3 ;
		else if( *p == '4' ) n = 4 ;
		else if( *p == '5' ) n = 5 ;
		else if( *p == '6' ) n = 6 ;
		else if( *p == '7' ) n = 7 ;
		else if( *p == '8' ) n = 8 ;
		else if( *p == '9' ) n = 9 ;
		else if( *p == 'a' || *p == 'A' ) n = 10 ;
		else if( *p == 'b' || *p == 'B' ) n = 11 ;
		else if( *p == 'c' || *p == 'C' ) n = 12 ;
		else if( *p == 'd' || *p == 'D' ) n = 13 ;
		else if( *p == 'e' || *p == 'E' ) n = 14 ;
		else if( *p == 'f' || *p == 'F' ) n = 15 ;
		else break ;
		if( static_cast<unsigned>(n) >= base )
			break ;
		if( scale > 1 && n && checkedMultiply<T>(n,scale) )
			overflow = true ;
		if( !first && checkedMultiply<T>(result,base) )
			overflow = true ;
		if( checkedAdd<T>( result , n ) )
			overflow = true ;
	}
	return overflow ? std::numeric_limits<T>::max() : result ;
}

template <typename T>
bool G::StringValue::detail::checkedMultiply( T & a , T b ) noexcept
{
	// (ckd_mul<>() is c++26)
	static constexpr T mid = std::numeric_limits<T>::max() >> (sizeof(T)*4U) ;
	const bool safe = a < mid && b < mid ; // optimisation (?) wrt. division
	T old_a = a ;
	a *= b ;
	return safe ? false : ( b && (a/b) != old_a ) ;
}

template <typename T>
bool G::StringValue::detail::checkedAdd( T & a , T b ) noexcept
{
	// (ckd_add<>() is c++26)
	T old_a = a ;
	a += b ;
	return a < old_a ;
}

#endif
