//
// Copyright (C) 2001 Graeme Walker <graeme_walker@users.sourceforge.net>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
// ===
//
// gnumber.h
//

#ifndef G_NUMBER_H
#define G_NUMBER_H

#include "gdef.h"
#include <string>

namespace G
{
	class Number ;
} ;

// Class: G::Number
// Description: A Number object represents an unsigned integer
// of up to 64 bits. The implementation is very old and not complete.
//
// See also: a64l(3c), LONGLONG, uint64_t, u_longlong_t
//
class G::Number
{
public:
	Number() ;
		// Default constructor. The value is set to zero.

	Number( g_uint32_t n32 ) ;
		// Constructor taking one 32-bit argument.

	Number( g_uint32_t high , g_uint32_t low ) ;
		// Constructor taking two 32-bit arguments.

	Number( const Number &other ) ;
		// Copy constructor.

	~Number() ;
		// Destructor.

	g_uint32_t value() const ;
		// Returns the numeric value. Returns the maximum
		// 32-bit value if the value is too big for a g_uint32_t.
		// See big(). (This behaviour is generally preferable to
		// truncating the value to 32 bits since a very big
		// number can suddenly become a small number. Other
		// member functions are available to do truncation.)

	unsigned long value( unsigned long *ulp ) ;
		// Returns (by value and reference) the value
		// as an unsigned long. The value returned is
		// ULONG_MAX if the number is too big for an
		// unsigned long. See big(unsigned long*) and
		// operator unsigned long().

	long value( long *lp ) ;
		// Returns (by value and reference) the value
		// as a long integer. The value returned is
		// LONG_MAX if the number is too big for a long.
		// See big(long*).

	operator unsigned long() ;
		// Cast operator. The value returned is ULONG_MAX if
		// the number is too big for an unsigned long. See
		// big(unsigned long*) and value(unsigned long*).

	operator double() ;
		// Cast operator returning a double precision
		// floating point value.

	bool big() const ;
		// Returns true if the number is bigger than
		// 32 bits. See value().

	bool big( long *dummy ) const ;
		// Returns true if the number is too big
		// for a long. See operator long().
		// The dummy parameter is ignored.

	bool big( unsigned long *dummy ) const ;
		// Returns true if the number is too big
		// for an unsigned long. See operator
		// unsigned long(). The dummy parameter
		// is ignored.

	Number &operator=( const Number &other ) ;
		// Assignment operator.

	Number &to48bits() ;
		// Truncates the value to 48 bits. The top 16 bits
		// are zeroed.

	g_uint32_t high() const ;
		// Returns the most significant 32 bits.

	g_uint32_t low() const ;
		// Returns the least significant 32 bits.

	Number  operator+ ( const Number &addend ) const ;
		// Addition operator.

	Number &operator+=( const Number &addend ) ;
		// Self-addition operator.

	Number  operator- ( const Number &subtrahend ) const ;
		// Subtraction operator.

	Number &operator-=( const Number &subtrahend ) ;
		// Self-subtraction operator.

	Number  operator* ( const Number &multiplicand ) const ;
		// Multiplcation operator.

	Number &operator*=( const Number &multiplicand ) ;
		// Self-multiplication operator.

	Number  operator/ ( g_uint16_t divisor ) const ;
		// Division operator.

	Number &operator/=( g_uint16_t divisor ) ;
		// Self-division operator.

	Number  operator% ( g_uint16_t divisor ) const ;
		// Modulo operator.

	Number &operator%=( g_uint16_t divisor ) ;
		// Self-modulo operator.

	Number &operator|=( const Number &other ) ;
		// Bitwise OR operator.

	void lshift( unsigned places ) ;
		// Left-shifts this number by the given number of bits.

	void lshift32() ;
		// Left-shifts this number 32 bits.

	void rshift( unsigned places ) ;
		// Right-shifts this number by the given number of bits.

	void rshift32() ;
		// Right-shifts this number 32 bits.

	Number &operator<<=( unsigned places ) ;
		// Left-shift operator.

	Number &operator>>=( unsigned places ) ;
		// Right-shift operator.

	Number operator++(int) ;
		// Post-increment operator.

	Number operator--(int) ;
		// Post-decrement operator.

	Number &operator++() ;
		// Pre-increment operator.

	Number &operator--() ;
		// Post-increment operator.

	bool operator==( const Number &other ) const ;
		// Equality comparison operator.

	bool operator!=( const Number &other ) const ;
		// Inequality comparison operator.

	bool operator<( const Number &other ) const ;
		// Less-than comparison operator.

	bool operator<=( const Number &other ) const ;
		// Less-than-or-equal comparison operator.

	bool operator>( const Number &other ) const ;
		// Greater-than comparison operator.

	bool operator>=( const Number &other ) const ;
		// Greater-than-or-equal comparison operator.

	friend std::ostream &operator<<( std::ostream &stream , const Number n ) ;
		// Global function which streams out a Number object.

	std::string displayString() const ;
		// Returns a printable string representation.

private:
	static void divide16( const Number &dividend , g_uint16_t divisor , Number &quot , Number &rem ) ;

private:
	g_uint32_t m_high ;
	g_uint32_t m_low ;
} ;

#endif

