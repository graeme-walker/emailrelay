//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
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
/// Unvalued options (eg. "--debug") can be be explicitly on (eg. "--debug=yes")
/// or off ("--debug=no"); the latter are typically ignored.
///
class G::OptionValue
{
public:
	OptionValue() ;
		///< Default constructor for a valueless value.

	explicit OptionValue( const std::string & s , std::size_t count = 1U ) ;
		///< Constructor for a valued value.
		///< Precondition: !s.empty()

	static OptionValue on() ;
		///< A factory function for an unvalued option-enabled option.

	static OptionValue off() ;
		///< A factory function for an unvalued option-disabled option.

	bool isOn() const noexcept ;
		///< Returns true if on().

	bool isOff() const noexcept ;
		///< Returns true if off().

	std::string value() const ;
		///< Returns the value as a string.

	std::string_view valueref() const noexcept ;
		///< Exposes the value as a string view.

	bool operator==( std::string_view ) const noexcept ;
		///< Returns true if the given string matches value().

	bool numeric() const noexcept ;
		///< Returns true if value() is an unsigned integer.

	unsigned int number( unsigned int default_ = 0U ) const ;
		///< Returns value() as an unsigned integer.
		///< Returns the default if not numeric().

	size_t count() const noexcept ;
		///< Returns an instance count that is one by default.

	void increment() noexcept ;
		///< Increments the instance count().

private:
	bool m_on_off {false} ;
	std::size_t m_count {1U} ;
	std::string m_value ;
} ;

inline
G::OptionValue::OptionValue() :
	m_on_off(true) ,
	m_value(G::Str::positive())
{
}

inline
G::OptionValue::OptionValue( const std::string & s , std::size_t count ) :
	m_count(count) ,
	m_value(s)
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
	OptionValue v ;
	v.m_value = G::Str::negative() ;
	return v ;
}

inline
bool G::OptionValue::isOn() const noexcept
{
	return m_on_off && G::Str::isPositive(m_value) ;
}

inline
bool G::OptionValue::isOff() const noexcept
{
	return m_on_off && G::Str::isNegative(m_value) ;
}

inline
std::string G::OptionValue::value() const
{
	return m_value ;
}

inline
std::string_view G::OptionValue::valueref() const noexcept
{
	return {m_value} ;
}

inline
bool G::OptionValue::operator==( std::string_view s ) const noexcept
{
	return s == m_value ;
}

inline
bool G::OptionValue::numeric() const noexcept
{
	return !m_on_off && G::Str::isUInt(m_value) ;
}

inline
unsigned int G::OptionValue::number( unsigned int default_ ) const
{
	return numeric() ? G::Str::toUInt(m_value) : default_ ;
}

inline
std::size_t G::OptionValue::count() const noexcept
{
	return m_count ;
}

inline
void G::OptionValue::increment() noexcept
{
	m_count++ ;
}

#endif
