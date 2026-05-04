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
/// \file goptions.h
///

#ifndef G_OPTIONS_H
#define G_OPTIONS_H

#include "gdef.h"
#include "gstringarray.h"
#include "gexception.h"
#include "goption.h"
#include <string>

namespace G
{
	class Options ;
}

//| \class G::Options
/// A class to assemble a list of command-line options into a
/// command-line specification and then provide access by name.
///
class G::Options
{
public:
	G_EXCEPTION( InvalidSpecification , tx("invalid options specification string") )

	explicit Options( const std::string & spec , char sep_major = '|' , char sep_minor = '!' , char escape = '^' ) ;
		///< Constructor taking a specification string.
		///<
		///< Uses specifications like
		///< "p!port!the port! for listening!1!port!1|v!verbose!more logging! and help!0!!1!tag1!tag2"
		///< made up of (1) an optional single-character-option-letter,
		///< (2) a multi-character-option-name (3) an option-description,
		///< (4) optional option-description-extra text, (5) a value-type
		///< (with '0' for unvalued, '1' for a single value, '2' for a
		///< comma-separated list (possibly multiple times), or '01'
		///< for a defaultable single value) or (6) a value-description
		///< (unless unvalued), (7) a level enumeration, and optional
		///< trailing tags.
		///<
		///< Typically mainstream options are given a level of 1, and
		///< obscure ones level 2 and above. If the option-description
		///< field is empty or if the level is zero then the option
		///< is hidden.
		///<
		///< The first tag (if any) is the main tag, corresponding to a
		///< sub-heading when generating documentation.

	Options() ;
		///< Default constructor for no options.

	static void add( Options & , char c , const char * name , const char * text ,
        const char * more , Option::Multiplicity m , const char * argname ,
		unsigned int level , unsigned int main_tag_bit , unsigned int tag_bits = 0U ) ;
			///< A convenience function that constructs an Option object with
			///< the trailing arguments and then calls add(Option) on the
			///< given Options object. The 'text' string is passed through
			///< G::gettext() and should therefore normally be marked
			///< for translation with G::tx(). The tag parameters are
			///< both bit-masks, with only one bit set in the main tag.

	void add( const Option & ) ;
		///< Adds one component of the specification.

	const std::vector<Option> & list() const ;
		///< Exposes the sorted list of option structures.

	std::string lookup( char c ) const ;
		///< Converts from short-form option character to the corresponding
		///< long-form name. Returns the empty string if none.

	bool valid( std::string_view ) const ;
		///< Returns true if the long-form option name is valid.

	const Option * find( std::string_view ) const ;
		///< Returns a pointer to the option with a matching
		///< long-form name. Returns nullptr if none.

	bool visible( std::string_view name , unsigned int level , bool level_exact = false ) const ;
		///< Returns true if the option is visible at the given level.
		///< Deliberately hidden options at level zero are never
		///< visible(). Returns false if not a valid() name .

	bool visible( std::string_view name ) const ;
		///< Returns true if the option is visible. Returns false if
		///< the option is hidden or the name is not valid().

	bool singlevalued( char ) const ;
		///< Returns true if the given short-form option is valid
		///< and G::Option::singlevalued().

	bool singlevalued( std::string_view ) const ;
		///< Returns true if the given long-form option is valid
		///< and G::Option::singlevalued().

	bool valued( char ) const ;
		///< Returns true if the given short-form option is valid
		///< and G::Option::valued().

	bool valued( std::string_view ) const ;
		///< Returns true if the given long-form option is valid
		///< and G::Option::valued().

	bool multivalued( char ) const ;
		///< Returns true if the given short-form option is valid
		///< and G::Option::multivalued().

	bool multivalued( std::string_view ) const ;
		///< Returns true if the given long-form option is valid
		///< and G::Option::multivalued().

	bool unvalued( std::string_view ) const ;
		///< Returns true if the given long-form option is valid
		///< and G::Option::unvalued().

	bool defaulting( std::string_view ) const ;
		///< Returns true if the given long-form option is valid
		///< and G::Option::defaulting().

	bool defaulting( char ) const ;
		///< Returns true if the given short-form option is valid
		///< and G::Option::defaulting().

private:
	using List = std::vector<Option> ;
	void parseSpec( const std::string & spec , char , char , char ) ;

private:
	List m_list ;
} ;

#endif
