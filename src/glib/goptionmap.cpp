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
/// \file goptionmap.cpp
///

#include "gdef.h"
#include "goptionmap.h"
#include "gstringtoken.h"
#include <algorithm>
#include <iterator>
#include <type_traits>
#include <limits>

namespace G
{
	namespace OptionMapImp
	{
		// (std::find_if() says nothing about noexcept, so do it ourselves)
		using const_iterator = OptionMap::const_iterator ;
		template <typename T> static const_iterator find_if( const_iterator p , const_iterator end , T pred ) noexcept
		{
			static_assert( std::is_pointer<const_iterator>::value , "" ) ; // so noexcept(++p)
			static_assert( noexcept(*p) , "" ) ;
			//static_assert( noexcept(pred(*p)) , "" ) ; // c++17
			static_assert( noexcept(p!=end) , "" ) ;
			for( ; p != end ; ++p )
			{
				if( pred(*p) )
					return p ;
			}
			return end ;
		}
		template <typename T> static bool any_of( const_iterator begin , const_iterator end , T pred ) noexcept
		{
			return find_if( begin , end , pred ) != end ;
		}
		static bool isOn( const OptionMap::Pair & p ) noexcept
		{
			static_assert( noexcept(p.second.isOn()) , "" ) ;
			return p.second.isOn() ;
		}
		static bool isOff( const OptionMap::Pair & p ) noexcept
		{
			static_assert( noexcept(p.second.isOff()) , "" ) ;
			return p.second.isOff() ;
		}
	}
}

G::OptionMap::Range G::OptionMap::findRange( std::string_view key ) const noexcept
{
	// (not std::equal_range() here because constructing the c++11 comparison value can throw)
	namespace imp = G::OptionMapImp ;
	auto first = imp::find_if( cbegin(m_list) , cend(m_list) ,
		[key](const value_type &v_) noexcept {
			static_assert( noexcept(key<=v_.first) , "" ) ;
			return key <= v_.first ;
		} ) ;
	auto second = imp::find_if( first , cend(m_list) ,
		[key](const value_type &v_) noexcept {
			static_assert( noexcept(key<=v_.first) , "" ) ;
			return key < v_.first ;
		} ) ;
	return {first,second} ;
}

G::OptionMap::Range_ G::OptionMap::findRange( std::string_view key )
{
	return std::equal_range( m_list.begin() , m_list.end() , value_type(sv_to_string(key),OptionValue()) ,
		[](const value_type & v1_,const value_type & v2_){return v1_.first < v2_.first;} ) ;
}

void G::OptionMap::insert( const value_type & key_value )
{
	auto range = findRange( key_value.first ) ;
	if( range.first == range.second )
		m_list.insert( range.first , key_value ) ;
	else
		m_list.insert( range.second , key_value ) ;
}

void G::OptionMap::replace( std::string_view key , const std::string & value )
{
	auto range = findRange( key ) ;
	if( range.first != range.second )
		m_list.erase( range.first , range.second ) ;
	insert( value_type(sv_to_string(key),OptionValue(value)) ) ;
}

G::OptionMap::const_iterator G::OptionMap::begin() const noexcept
{
	return cbegin( m_list ) ;
}

G::OptionMap::const_iterator G::OptionMap::end() const noexcept
{
	return cend( m_list ) ;
}

G::OptionMap::const_iterator G::OptionMap::findFirst( std::string_view key ) const noexcept
{
	auto range = findRange( key ) ;
	return range.first == range.second ? cend(m_list) : range.first ;
}

void G::OptionMap::clear()
{
	m_list.clear() ;
}

void G::OptionMap::clearOffValues()
{
	m_list.erase( std::remove_if( m_list.begin() , m_list.end() ,
		[](const value_type &p_){return p_.second.isOff();} ) , m_list.end() ) ;
}

bool G::OptionMap::contains( std::string_view key ) const noexcept
{
	auto range = findRange( key ) ;
	return range.first != range.second ;
}

bool G::OptionMap::containsOn( std::string_view key ) const noexcept
{
	namespace imp = G::OptionMapImp ;
	auto range = findRange( key ) ;
	return imp::any_of( range.first , range.second , &imp::isOn ) ;
}

bool G::OptionMap::containsOff( std::string_view key ) const noexcept
{
	namespace imp = G::OptionMapImp ;
	auto range = findRange( key ) ;
	return imp::any_of( range.first , range.second , &imp::isOff ) ;
}

std::size_t G::OptionMap::count( std::string_view key ) const noexcept
{
	std::size_t n = 0U ;
	auto range = findRange( key ) ;
	for( auto p = range.first ; p != range.second ; ++p )
	{
		if( !(*p).second.isOff() )
			n++ ;
	}
	return n ;
}

std::string G::OptionMap::value( std::string_view key , std::string_view default_ ,
	std::string_view sep ) const
{
	if( sep.empty() )
		sep = std::string_view( "," , 1U ) ;

	auto range = findRange( key ) ;

	// (optimise the common cases)
	if( range.first == range.second )
		return sv_to_string( default_ ) ;
	else if( std::next(range.first) == range.second && !(*range.first).second.isOff() )
		return (*range.first).second.value() ;

	std::string result ;
	m_seen.clear() ;
	for( auto p = range.first ; p != range.second ; ++p )
	{
		if( (*p).second.isOff() )
			continue ;

		std::string_view s = (*p).second.valueref() ;
		if( std::find(m_seen.begin(),m_seen.end(),s) != m_seen.end() )
			continue ;
		m_seen.push_back( s ) ;

		if( !result.empty() )
			result.append( sep.data() , sep.size() ) ;
		result.append( s.data() , s.size() ) ;
	}
	return result ;
}

G::OptionMap::NumberPair G::OptionMap::number( std::string_view key , unsigned int default_ ) const noexcept
{
	// if it's a number it has to be single-valued, so use findRange()
	// directly rather than value() (which might throw)
	auto range = findRange( key ) ;
	if( range.first == range.second )
		return {ValueStatus::missing,default_} ;
	if( std::next(range.first) != range.second )
		return {ValueStatus::multiple,default_} ;

	static_assert( std::is_nothrow_copy_constructible<NumberPair>::value , "" ) ;
	std::string_view value = (*range.first).second.valueref() ;
	static_assert( noexcept(G::Str::isUInt(value)) , "" ) ;
	static_assert( noexcept(G::Str::toUInt(value,0U)) , "" ) ;
	if( !value.empty() && G::Str::isUInt(value) )
		return {ValueStatus::ok,G::Str::toUInt(value,0U)} ;
	else
		return {ValueStatus::invalid,default_} ;
}

G::OptionMap::const_iterator G::OptionMap::cbegin( const List & list ) noexcept
{
	// not std::cbegin(list) so that iterator operations are noexcept
	return list.empty() ? nullptr : list.data() ;
}

G::OptionMap::const_iterator G::OptionMap::cend( const List & list ) noexcept
{
	return list.empty() ? nullptr : ( list.data() + list.size() ) ;
}

