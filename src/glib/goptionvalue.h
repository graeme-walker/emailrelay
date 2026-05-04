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
/// \file goptionvalue.h
///

#ifndef G_OPTION_VALUE_H
#define G_OPTION_VALUE_H

#include "gdef.h"
#include "gstr.h"
#include "gstringview.h"
#include <string>

namespace G
{
	class OptionValue ;
}

//| \class G::OptionValue
/// A simple structure encapsulating the value of a command-line option.
///
class G::OptionValue
{
public:
	OptionValue() ;
		///< Default constructor for an on/off 'on' object.
		///< Postcondition: isOnOff() && isOn() && value() == Str::positive()

	explicit OptionValue( std::string_view ) ;
		///< Constructor for a valued object.
		///< Precondition: !s.empty()

	static OptionValue on() ;
		///< A factory function for an on/off 'on' object.
		///< Postcondition: isOnOff() && isOn() && value() == Str::positive()

	static OptionValue off() ;
		///< A factory function for an on/off 'off' object.
		///< Postcondition: isOnOff() && isOff() && value() == Str::negative()

	bool isOnOff() const noexcept ;
		///< Returns true if default constructed or constructed by on() or off().

	bool isOn() const noexcept ;
		///< Returns true if default constructed or constructed by on().

	bool isOff() const noexcept ;
		///< Returns true if constructed by off().

	std::string value() const ;
		///< Returns the value as a string. Returns Str::positive() or
		///< Str::negative() if isOnOff().

	std::string_view valueref() const & noexcept ;
		///< Exposes the value() as a string view.

	bool numeric() const noexcept ;
		///< Returns true if value() is an unsigned integer.

	unsigned int number( unsigned int default_ = 0U ) const ;
		///< Returns value() as an unsigned integer.
		///< Returns the default if not numeric().

public:
	std::string_view valueref() const && = delete ;

private:
	bool m_on {false} ;
	bool m_off {false} ;
	std::string m_value ;
} ;

inline
G::OptionValue::OptionValue() :
	m_on(true) ,
	m_value(sv_to_string(G::Str::positive()))
{
}

inline
G::OptionValue::OptionValue( std::string_view sv ) :
	m_value(sv_to_string(sv))
{
}

inline
G::OptionValue G::OptionValue::on()
{
	return {} ;
}

inline
G::OptionValue G::OptionValue::off()
{
	OptionValue v( G::Str::negative() ) ;
	v.m_off = true ;
	return v ;
}

inline
bool G::OptionValue::isOnOff() const noexcept
{
	return m_on || m_off ;
}

inline
bool G::OptionValue::isOn() const noexcept
{
	return m_on ;
}

inline
bool G::OptionValue::isOff() const noexcept
{
	return m_off ;
}

inline
std::string G::OptionValue::value() const
{
	return m_value ;
}

inline
std::string_view G::OptionValue::valueref() const & noexcept
{
	static_assert( noexcept(std::string_view(m_value)) , "" ) ;
	return {m_value} ;
}

inline
bool G::OptionValue::numeric() const noexcept
{
	return !m_on && !m_off && Str::isUInt(m_value) ;
}

inline
unsigned int G::OptionValue::number( unsigned int default_ ) const
{
	return numeric() ? Str::toUInt(m_value) : default_ ;
}

#endif
