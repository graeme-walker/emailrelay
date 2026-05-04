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
/// \file goption.h
///

#ifndef G_OPTION_H
#define G_OPTION_H

#include "gdef.h"
#include "gstringview.h"
#include "gstringarray.h"
#include <string>
#include <utility>

namespace G
{
	class Option ;
}

//| \class G::Option
/// A structure representing the form of a command-line option, typically 
/// used as part of a G::Options command-line specification.
///
class G::Option
{
public:
	enum class Multiplicity { zero , zero_or_one , one , many , error } ;

	Option( char c , std::string_view name , std::string_view description ,
		std::string_view description_extra , Multiplicity ,
		std::string_view value_description , unsigned int level ,
		unsigned int main_tag = 0U , unsigned int tag_bits = 0U ) ;
			///< Constructor. The option is hidden if the description
			///< is empty or if the level is zero (see visible()).
			///<
			///< If description_extra is exactly two characters
			///< (a split character and an escape character) then
			///< they are used to split the description argument
			///< into description and description_extra. This
			///< allows the combined string to be translated
			///< as a whole.

	static Multiplicity decode( const std::string & ) ;
		///< Decodes a multiplicity string into its enumeration.
		///< Returns 'error' on error.

	const std::string & name() const noexcept ;
		///< Name accessor.

	char c() const noexcept ;
		///< Returns the switch character or '\0' if none.

	const std::string & description() const noexcept ;
		///< Description accessor.

	const std::string & description_extra() const noexcept ;
		///< Description-extra accessor.

	const std::string & value_description() const noexcept ;
		///< Value-description accessor.

	unsigned int level() const noexcept ;
		///< Returns the level.

	bool unvalued() const noexcept ;
		///< Returns true if multiplicity is zero.

	bool defaulting() const noexcept ;
		///< Returns true if multiplicity is zero-or-one.

	bool singlevalued() const noexcept ;
		///< Returns true if multiplicity is one.

	bool multivalued() const noexcept ;
		///< Returns true if multiplicity is many.

	bool valued() const noexcept ;
		///< Returns true if defaulting(), singlevalued() or multivalued().

	bool visible() const noexcept ;
		///< Returns true if not hidden.

	bool visible( std::pair<unsigned,unsigned> level_range , unsigned int main_tag = 0U , unsigned int tag_bits = 0U ) const noexcept ;
		///< Returns true if in the given level range, not hidden,
		///< and with a relevant tag.

private:
	char m_c ;
	std::string m_name ;
	std::string m_description ;
	std::string m_description_extra ;
	Multiplicity m_value_multiplicity ;
	bool m_hidden ;
	std::string m_value_description ;
	unsigned int m_level ;
	unsigned int m_main_tag ; // principal category
	unsigned int m_tag_bits ; // all categories
} ;

// clang-format off
inline const std::string & G::Option::name() const noexcept { return m_name ; }
inline char G::Option::c() const noexcept { return m_c ; }
inline const std::string & G::Option::description() const noexcept { return m_description ; }
inline const std::string & G::Option::description_extra() const noexcept { return m_description_extra ; }
inline const std::string & G::Option::value_description() const noexcept { return m_value_description ; }
inline unsigned int G::Option::level() const noexcept { return m_level ; }
inline bool G::Option::unvalued() const noexcept { return m_value_multiplicity == Option::Multiplicity::zero ; }
inline bool G::Option::defaulting() const noexcept { return m_value_multiplicity == Option::Multiplicity::zero_or_one ; }
inline bool G::Option::singlevalued() const noexcept { return m_value_multiplicity == Option::Multiplicity::one ; }
inline bool G::Option::multivalued() const noexcept { return m_value_multiplicity == Option::Multiplicity::many ; }
inline bool G::Option::valued() const noexcept { return defaulting() || singlevalued() || multivalued() ; }
inline bool G::Option::visible() const noexcept { return visible( {1U,99U} ) ; }
// clang-format on

inline bool G::Option::visible( std::pair<unsigned,unsigned> level_range ,
	unsigned int main_tag_in , unsigned int tag_bits_in ) const noexcept
{
	return
		!m_hidden &&
		m_level >= level_range.first &&
		m_level <= level_range.second &&
		( main_tag_in == 0U || main_tag_in == m_main_tag ) &&
		( tag_bits_in == 0U || ( tag_bits_in & m_tag_bits ) != 0U ) ;
}

#endif
