//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file goptional.h
///

#ifndef G_OPTIONAL_H
#define G_OPTIONAL_H

#include "gdef.h"

#if GCONFIG_HAVE_CXX_OPTIONAL

#include <optional>

#else

#include <utility>
#include <stdexcept>

namespace G
{
	template <typename T> class optional ;
}

namespace std /// NOLINT
{
	using G::optional ;
}

//| \class G::optional
/// A class template like a simplified c++17 std::optional.
///
template <typename T>
class G::optional
{
public:
	optional() noexcept(noexcept(T())) ;
		///< Default constructor for no value.

	explicit optional( const T & ) ;
		///< Constructor for a defined value.

	void reset() ;
		///< Clears the value.

	bool has_value() const noexcept ;
		///< Returns true if a defined value.

	explicit operator bool() const noexcept ;
		///< Returns true if a defined value.

	const T & value() const ;
		///< Returns the value.

	T value_or( const T & ) const ;
		///< Returns the value or a default.

	optional<T> & operator=( const T & ) ;
		///< Assignment for a defined value.

public:
	~optional() = default ;
	optional( const optional & ) = default ;
	optional( optional && ) noexcept = default ;
	optional & operator=( const optional & ) = default ;
	optional & operator=( optional && ) noexcept = default ;

private:
	void doThrow() const ;

private:
	T m_value {} ;
	bool m_has_value {false} ;
} ;

template <typename T>
G::optional<T>::optional() noexcept(noexcept(T()))
= default ;

template <typename T>
G::optional<T>::optional( const T & t ) :
	m_value(t) ,
	m_has_value(true)
{
}

template <typename T>
void G::optional<T>::reset()
{
	m_has_value = false ;
}

template <typename T>
bool G::optional<T>::has_value() const noexcept
{
	return m_has_value ;
}

template <typename T>
G::optional<T>::operator bool() const noexcept
{
	return m_has_value ;
}

template <typename T>
const T & G::optional<T>::value() const
{
	if( !m_has_value ) doThrow() ;
	return m_value ;
}

template <typename T>
void G::optional<T>::doThrow() const
{
	throw std::runtime_error( "bad optional access" ) ;
}

template <typename T>
T G::optional<T>::value_or( const T & default_ ) const
{
	return m_has_value ? m_value : default_ ;
}

template <typename T>
G::optional<T> & G::optional<T>::operator=( const T & t )
{
	m_value = t ;
	m_has_value = true ;
	return *this ;
}

#endif

#endif
