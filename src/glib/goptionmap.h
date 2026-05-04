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
/// \file goptionmap.h
///

#ifndef G_OPTION_MAP_H
#define G_OPTION_MAP_H

#include "gdef.h"
#include "goptionvalue.h"
#include "gdatetime.h"
#include "gstringview.h"
#include <string>
#include <vector>
#include <algorithm>

namespace G
{
	class OptionMap ;
}

//| \class G::OptionMap
/// A multimap-like container for command-line options and their values.
/// The values are G::OptionValue objects, so they can be valued with a
/// string value or unvalued with an on/off status. Equal values are
/// stored separately but elided by value(). Values that are 'off'
/// are ignored by value() and count(). Normally populated by
/// G::OptionParser.
///
/// The interface is non-throwing for lookup because it can be an
/// important code-size optimisation when initialising large
/// configuration structures. The implementation therefore avoids
/// standard iterators and algorithms where necessary.
///
class G::OptionMap
{
public:
	using Pair = std::pair<std::string,OptionValue> ;
	using List = std::vector<Pair> ;
	using value_type = Pair ;
	using iterator = List::iterator ;
	using const_iterator = const Pair * ;
	enum class ValueStatus { ok , missing , multiple , invalid } ;
	using NumberPair = std::pair<ValueStatus,unsigned int> ;

public:
	void insert( const value_type & ) ;
		///< Inserts the key/value pair into the map. The ordering of
		///< values with the same key is in the order of insert()ion.

	void replace( std::string_view key , const std::string & value ) ;
		///< Replaces all matching values with a single value.

	const_iterator begin() const noexcept ;
		///< Returns the begin const iterator.

	const_iterator end() const noexcept ;
		///< Returns the off-the-end const iterator.

	const_iterator findFirst( std::string_view ) const noexcept ;
		///< Finds the first map entry with the given key.

	void clear() ;
		///< Clears the map.

	bool contains( std::string_view key ) const noexcept ;
		///< Returns true if the map contains the given key, including
		///< 'off' values.

	bool containsOn( std::string_view key ) const noexcept ;
		///< Returns true if the map contains an 'on' value
		///< with the given key.

	bool containsOff( std::string_view key ) const noexcept ;
		///< Returns true if the map contains an 'off' value
		///< with the given key.

	std::size_t count( std::string_view key ) const noexcept ;
		///< Returns the count of entries with the given key, ignoring
		///< 'off' values.

	std::string value( std::string_view key , std::string_view default_ = {} ,
		std::string_view sep = {} ) const ;
			///< Returns a value for the given key, ignoring 'off' values and
			///< with concatentation into a comma-separated list if more than
			///< one (with no escaping). Insert order is preserved. Repeat
			///< values are elided. Returns the empty string if no match.

	NumberPair number( std::string_view key , unsigned int default_ ) const noexcept ;
		///< Returns value() as a number, together with a status.
		///< Returns the default number-of-seconds value if the value
		///< status is not ok.

	void clearOffValues() ;
		///< Removes options where the value isOff().

private:
	using Range = std::pair<const_iterator,const_iterator> ;
	using Range_ = std::pair<iterator,iterator> ;
	Range findRange( std::string_view key ) const noexcept ;
	Range_ findRange( std::string_view key ) ;
	static const_iterator cbegin( const List & ) noexcept ;
	static const_iterator cend( const List & ) noexcept ;
	static unsigned int mulsat( unsigned int , unsigned int ) noexcept ;

private:
	List m_list ;
	mutable std::vector<std::string_view> m_seen ;
} ;

#endif
