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
// gnumber.cc
//

#include "gdef.h"
#include "gnumber.h"
#include "gdebug.h"
#include <stdio.h>
#include <math.h>
#include <limits.h>

const g_uint32_t bit31 = 0x80000000L ;

inline bool msb( g_uint32_t n )
{
	return !!( n & bit31 ) ;
} ;

G::Number::~Number()
{
}

G::Number::Number() :
	m_high(0) ,
	m_low(0)
{
}

G::Number::Number( g_uint32_t n32 ) :
	m_low(n32) ,
	m_high(0)
{
}

G::Number::Number( g_uint32_t high , g_uint32_t low ) :
	m_low(low) ,
	m_high(high)
{
}

G::Number::Number( const Number &other )
{
	m_low = other.m_low ;
	m_high = other.m_high ;
}

G::Number &G::Number::operator=( const Number &other )
{
	m_low = other.m_low ;
	m_high = other.m_high ;
	return *this ;
}

bool G::Number::big() const
{
	return m_high != 0 ;
}

bool G::Number::big( long* ) const
{
	// optimise for the common case
	if( sizeof(long) <= sizeof(g_uint32_t) )
	{
		return m_high != 0 || m_low > LONG_MAX ;
	}

	// or more generally...
	Number n = LONG_MAX ;
	return (*this) > n ;
}

bool G::Number::big( unsigned long* ) const
{
	if( sizeof(unsigned long) <= sizeof(g_uint32_t) )
	{
		return m_high != 0 || m_low > ULONG_MAX ;
	}

	Number n = ULONG_MAX ;
	return (*this) > n ;
}

g_uint32_t G::Number::value() const
{
	if( big() )
	{
		g_uint32_t rc = 0 ;
		rc = ~rc ;
		return rc ;
	}
	return m_low ;
}

unsigned long G::Number::value( unsigned long *p )
{
	if( big((unsigned long*)0) )
	{
		if( p != NULL ) *p = ULONG_MAX ;
		return ULONG_MAX ;
	}

	unsigned long ul = m_high ;
	ul <<= 32 ; // ignore warnings -- this is in case sizeof(long) > 32
	ul |= m_low ;
	if( p != NULL ) *p = ul ;
	return ul ;
}

G::Number::operator double()
{
	double d = m_high ;
	d *= static_cast<double>(0x10000L) ; // <<=16
	d *= static_cast<double>(0x10000L) ; // <<=16
	d += m_low ;
	return d ;
}

G::Number::operator unsigned long()
{
	return value( (unsigned long *)0 ) ;
}

long G::Number::value( long *p )
{
	if( big((long*)0) )
	{
		if( p != NULL ) *p = LONG_MAX ;
		return LONG_MAX ;
	}

	long l = m_high ;
	l <<= 32 ; // ignore warnings -- this is in case sizeof(long) > 32
	l |= m_low ;
	if( p != NULL ) *p = l ;
	return l ;
}

G::Number &G::Number::to48bits()
{
	m_high &= 0xffff ;
	return *this ;
}

g_uint32_t G::Number::high() const
{
	return m_high ;
}

g_uint32_t G::Number::low() const
{
	return m_low ;
}

void G::Number::lshift( unsigned places )
{
	if( places == 0 )
	{
	}
	else if( places < 32 )
	{
		m_high <<= places ;
		m_high |= (m_low >> (32-places)) ;
		m_low <<= places ;
	}
	else if( places == 32 )
	{
		lshift32() ;
	}
	else if( places < 64 )
	{
		m_high = m_low ;
		m_low = 0 ;
		m_high <<= ( places - 32 ) ;
	}
	else
	{
		m_high = m_low = 0 ;
	}
}

void G::Number::lshift32()
{
	m_high = m_low ;
	m_low = 0 ;
}

void G::Number::rshift( unsigned places )
{
	if( places == 0 )
	{
	}
	else if( places < 32 )
	{
		m_low >>= places ;
		m_low |= (m_high << (32-places)) ;
		m_high >>= places ;
	}
	else if( places == 32 )
	{
		rshift32() ;
	}
	else if( places < 64 )
	{
		m_low = m_high ;
		m_high = 0 ;
		m_low >>= ( places - 32 ) ;
	}
	else
	{
		m_high = m_low = 0 ;
	}
}

void G::Number::rshift32()
{
	m_low = m_high ;
	m_high = 0 ;
}

G::Number G::Number::operator++(int)
{
	Number old = (*this) ;
	++(*this) ;
	return old ;
}

G::Number G::Number::operator--(int)
{
	Number old = (*this) ;
	--(*this) ;
	return old ;
}

G::Number &G::Number::operator++()
{
	m_low++ ;
	if( m_low == 0 )
		m_high++ ;
	return *this ;
}

G::Number &G::Number::operator--()
{
	if( m_low == 0 )
		m_high-- ;
	m_low-- ;
	return *this ;
}

G::Number G::Number::operator+( const Number &other ) const
{
	Number result = (*this) ;
	result += other ;
	return result ;
}

G::Number G::Number::operator-( const Number &other ) const
{
	Number result = (*this) ;
	result -= other ;
	return result ;
}

G::Number G::Number::operator*( const Number &other ) const
{
	Number result = (*this) ;
	result *= other ;
	return result ;
}

G::Number G::Number::operator/( g_uint16_t divisor ) const
{
	Number result = (*this) ;
	result /= divisor ;
	return result ;
}

G::Number &G::Number::operator-=( const Number &other )
{
	if( &other == this )
	{
		m_high = m_low = 0 ;
		return *this ;
	}

	// create two's complement of other..
	Number copy = other ;
	copy.m_high = ~copy.m_high ;
	copy.m_low = ~copy.m_low ;
	++copy ;

	// ..and add
	(*this) += copy ;
	return *this ;
}

G::Number &G::Number::operator+=( const Number &other )
{
	if( &other == this )
	{
		m_high <<= 1 ;
		m_high += msb( m_low ) ;
		m_low <<= 1 ;
		return *this ;
	}

	bool a = msb( m_low ) ;
	bool b = msb( other.m_low ) ;

	g_uint32_t new_low = m_low + other.m_low ;
	bool c = msb( new_low ) ;
	m_low = new_low ;

	bool carry = ( a && b ) | ( (a ^ b) && !c ) ;

	m_high += other.m_high ;
	if( carry )
		m_high++ ;

	return *this ;
}

G::Number &G::Number::operator/=( g_uint16_t divisor )
{
	Number remainder ;
	Number quotient ;
	divide16( (*this) , divisor , quotient , remainder ) ;
	(*this) = quotient ;
	return *this ;
}

G::Number &G::Number::operator%=( g_uint16_t divisor )
{
	Number remainder ;
	Number quotient ;
	divide16( (*this) , divisor , quotient , remainder ) ;
	(*this) = remainder ;
	return *this ;
}

void G::Number::divide16( const Number &const_dividend , g_uint16_t divisor , Number &quotient , Number &remainder )
{
	// division is complicated by the need for a double-width
	// accumulator ie. an accumulator double the width of the
	// divisor -- since we restrict ourselves to using built-in
	// types of 32 bits (for portability) this divisor is limited
	// to 16 bits. Full 64/64 bit division must be built using
	// this 64/16 bit division and a 128-bit accumulator.

	// the algorithm is basic long division -- consider dividing
	// a four digit (16-bit) hex number by a one-digit (4-bit)
	// hex number -- then scale the algorithm up to 64/16 bits.

	G_ASSERT( divisor != 0 ) ;
	quotient = 0 ;
	remainder = 0 ;
	Number accumulator = 0 ;
	Number dividend = const_dividend ;

	for( size_t i = 0 ; i < 4 ; i++ )
	{
		// shift the dividend into the accumulator
		accumulator <<= 16 ;
		accumulator.m_low |= (dividend.m_high >> 16) ;
		dividend <<= 16 ;
		G_ASSERT( accumulator.m_high == 0 ) ; // assert 32 bit accumulator

		// shift the partial quotient into the full quotient
		quotient <<= 16 ;
		quotient.m_low |= (accumulator.m_low / divisor) ;
		G_ASSERT( ((accumulator.m_low / divisor) >>16) == 0 ) ; // assert 16 bit partial quotient

		// set the accumulator to the partial remainder
		accumulator = (accumulator.m_low % divisor) ;
		G_ASSERT( (accumulator.m_low >> 16) == 0 ) ; // assert 16 bit accumulator
	}

	remainder = accumulator ;
}

G::Number &G::Number::operator*=( const Number &other )
{
	if( &other == this )
	{
		Number copy( other ) ;
		operator*=( copy ) ;
		return *this ;
	}

	// 16 * 16 -> 32
	if( m_high == 0 && other.m_high == 0 &&
		m_low <= 0xffff && other.m_low <= 0xffff )
	{
		m_low *= other.m_low ;
	}

	// 32 * 32 -> 64
	else if( m_high == 0 && other.m_high == 0 )
	{
		// four 16-bit numbers
		Number a( (m_low >> 16) & 0xffff ) ;
		Number b( m_low & 0xffff ) ;
		Number c( (other.m_low >> 16) & 0xffff ) ;
		Number d( other.m_low & 0xffff ) ;

		// one 32-bit partial product
		Number bd = b ; bd *= d ;

		// two 48-bit partial products
		Number cb = c ; cb *= b ; cb.lshift(16) ;
		Number ad = a ; ad *= d ; ad.lshift(16) ;

		// one 64-bit partial product
		Number ac = a ; ac *= c ; ac.lshift32() ;

		// 64-bit product
		Number product = bd ;
		product += cb ;
		product += ad ;
		product += ac ;
		m_low = product.m_low ;
		m_high = product.m_high ;
	}

	// 64 * 64 -> 64
	else
	{
		// four 32-bit numbers
		Number a( m_high ) ;
		Number b( m_low ) ;
		Number c( other.m_high ) ;
		Number d( other.m_low ) ;

		// three 64-bit partial products
		Number bd = b ; bd *= d ;
		Number cb = c ; cb *= b ; cb.lshift32() ;
		Number ad = a ; ad *= d ; ad.lshift32() ;

		// 64-bit product
		Number product = bd ;
		product += cb ;
		product += ad ;
		m_low = product.m_low ;
		m_high = product.m_high ;
	}

	return *this ;
}

bool G::Number::operator==( const Number &other ) const
{
	return m_high == other.m_high && m_low == other.m_low ;
}

bool G::Number::operator!=( const Number &other ) const
{
	return !( *this == other ) ;
}

bool G::Number::operator<( const Number &other ) const
{
	if( m_high == other.m_high )
		return m_low < other.m_low ;
	else
		return m_high < other.m_high ;
}

bool G::Number::operator<=( const Number &other ) const
{
	return ( *this < other ) || ( *this == other ) ;
}

bool G::Number::operator>( const Number &other ) const
{
	return !( *this <= other ) ;
}

bool G::Number::operator>=( const Number &other ) const
{
	return !( *this < other ) ;
}

G::Number &G::Number::operator|=( const Number &other )
{
	m_high |= other.m_high ;
	m_low |= other.m_low ;
	return *this ;
}

G::Number &G::Number::operator<<=( unsigned places )
{
	lshift( places ) ;
	return *this ;
}

G::Number &G::Number::operator>>=( unsigned places )
{
	rshift( places ) ;
	return *this ;
}

std::string G::Number::displayString() const
{
	std::stringstream ss ;
	if( m_high != 0 )
	{
		ss << m_high ;
		ss.width( 8U ) ;
		ss << m_low ;
	}
	else
	{
		ss << m_low ;
	}
	return ss.str() ;
}

namespace G
{
	std::ostream & operator<<( std::ostream & stream , const Number n )
	{
    		stream << n.displayString() ;
    		return stream ;
	}
} ;

