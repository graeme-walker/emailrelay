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
/// \file gdatetime.h
///

#ifndef G_DATE_TIME_H
#define G_DATE_TIME_H

#include "gdef.h"
#include "gexception.h"
#include <chrono>
#include <vector>
#include <string>
#include <iostream>
#include <type_traits>
#include <new>

namespace G
{
	namespace DateTime
	{
		G_EXCEPTION_CLASS( Error , tx("date/time error") )
		class SystemTime ;
		class TimerTime ;
		class TimeInterval ;
		class BrokenDownTime ;
		class Test ;
		class Zone ;
	}
}

//| \class G::DateTime::BrokenDownTime
/// An encapsulation of 'struct std::tm'.
///
class G::DateTime::BrokenDownTime
{
public:
	explicit BrokenDownTime( const struct std::tm & ) ;
		///< Constructor. The tm structure is assumed to have values
		///< within their valid ranges.

	BrokenDownTime( int year , int month , int day , int hh , int min , int ss ) ;
		///< Constructor. The parameter values are assumed to be
		///< within their valid ranges.

	constexpr bool valid() const noexcept ;
		///< Returns true if the main broken down values are in their
		///< valid ranges.

	static BrokenDownTime parse( const std::string & yyyy_mm_dd_hh_mm_ss ) ;
		///< Converts a suitably-formatted string to a BrokenDownTime.
		///< The string must be like "yyyy_mm_dd_hh_mm_ss" with arbitrary
		///< separation characters. Returns null() on error.

	static constexpr BrokenDownTime null() noexcept ;
		///< Factory function for an in-valid() object with bogus broken-down
		///< values.

	static BrokenDownTime midday( int year , int month , int day ) ;
		///< Factory function for midday on the given date.

	static BrokenDownTime midnight( int year , int month , int day ) ;
		///< Factory function for midnight starting the given date.

	static BrokenDownTime local( SystemTime ) ;
		///< Factory function for the locale-dependent local broken-down
		///< time of the given epoch time. See also SystemTime::local().

	static BrokenDownTime utc( SystemTime ) ;
		///< Factory function for the utc broken-down time of the given
		///< epoch time. See also SystemTime::utc().

	bool format( char * out , std::size_t out_size , const char * fmt ) const ;
		///< Puts the formatted date, including a terminating null character,
		///< into the given output buffer. Returns false if the output buffer
		///< is too small. Only simple non-locale-dependent format specifiers
		///< are allowed, and these allowed specifiers explicitly exclude
		///< '%z' and '%Z'.

	void format( std::vector<char> & out , const char * fmt ) const ;
		///< Overload for an output vector. Throws if the vector is
		///< too small for the result (with its null terminator).

	std::string str( const char * fmt ) const ;
		///< Returns the formatted date, with the same restrictions
		///< as format().

	std::string str() const ;
		///< Returns str() using a "%F %T" format.

	int year() const ;
		///< Returns the four-digit year value.

	int month() const ;
		///< Returns the 1..12 month value.

	int day() const ;
		///< Returns the 1..31 month-day value.

	int hour() const ;
		///< Returns the 0..23 hour value.

	int min() const ;
		///< Returns the 0..59 minute value.

	int sec() const ;
		///< Returns the 0..59 or 0..60 seconds value.

	int wday() const ;
		///< Returns week day where sunday=0 and saturday=6.

	std::time_t epochTimeFromUtc() const ;
		///< Converts this utc broken-down time into epoch time.
		///< Returns zero if not valid().

	std::time_t epochTimeFromLocal() const ;
		///< Uses std::mktime() to convert this locale-dependent
		///< local broken-down time into epoch time. Returns
		///< zero if not valid().

	bool sameMinute( const BrokenDownTime & ) const noexcept ;
		///< Returns true if this and the other broken-down
		///< times are the same, at minute resolution with
		///< no rounding.

	enum class Unit { day , month , year , hour , minute } ;

	BrokenDownTime next( Unit ) const ;
		///< Adds one time unit and returns the result.
		///< Year overflows are ignored.

	BrokenDownTime previous( Unit ) const ;
		///< Subtracts one time unit and returns the result.
		///< Year underflows are ignored.

	bool operator<( const BrokenDownTime & ) const noexcept ;
		///< Comparison operator.

	bool operator==( const BrokenDownTime & ) const noexcept ;
		///< Equality test.

	bool operator!=( const BrokenDownTime & ) const noexcept ;
		///< Inequality test.

private:
	constexpr BrokenDownTime() noexcept ;
	static constexpr bool valid( int n , int lo , int hi ) noexcept ;
	static constexpr int monthDays( int y , int m ) noexcept ;
	static constexpr int monthDays( const std::tm & tm ) noexcept ;

private:
	friend class G::DateTime::Test ;
	struct std::tm m_tm {} ;
} ;

//| \class G::DateTime::SystemTime
/// Represents a unix-epoch time with microsecond resolution.
///
class G::DateTime::SystemTime
{
public:
	using time_point_type = std::chrono::time_point<std::chrono::system_clock> ;
	static constexpr bool now_noexcept =
		noexcept(time_point_type::clock::now()) &&
		std::is_nothrow_assignable<time_point_type,time_point_type>::value && // NOLINT(misc-redundant-expression)
		std::is_nothrow_copy_constructible<time_point_type>::value ;

	static SystemTime now() noexcept(now_noexcept) ;
		///< Factory function for the current time.

	static SystemTime zero() ;
		///< Factory function for the start of the epoch.

	explicit SystemTime( std::time_t , unsigned long us = 0UL ) noexcept ;
		///< Constructor. The first parameter should be some
		///< large positive number. The second parameter can be
		///< more than 10^6.

	bool isZero() const ;
		///< Returns true if zero().

	bool sameSecond( const SystemTime & other ) const noexcept ;
		///< Returns true if this time and the other time are the same,
		///< at second resolution.

	BrokenDownTime local() const ;
		///< Returns the locale-dependent local broken-down time.

	BrokenDownTime utc() const ;
		///< Returns the utc broken-down time.

	unsigned int ms() const ;
		///< Returns the millisecond fraction.

	unsigned int us() const ;
		///< Returns the microsecond fraction.

	std::time_t s() const ;
		///< Returns the number of seconds since the start of the epoch.

	bool operator<( const SystemTime & ) const ;
		///< Comparison operator.

	bool operator<=( const SystemTime & ) const ;
		///< Comparison operator.

	bool operator==( const SystemTime & ) const ;
		///< Comparison operator.

	bool operator!=( const SystemTime & ) const ;
		///< Comparison operator.

	bool operator>( const SystemTime & ) const ;
		///< Comparison operator.

	bool operator>=( const SystemTime & ) const ;
		///< Comparison operator.

	void operator+=( TimeInterval ) ;
		///< Adds the given interval. Throws on overflow.

	void operator-=( TimeInterval ) ;
		///< Subtract the given interval. Throws on underflow.

	SystemTime operator+( TimeInterval ) const ;
		///< Returns this time with given interval added.
		///< Throws on overflow.

	TimeInterval operator-( const SystemTime & start ) const ;
		///< Returns the given start time's interval() compared
		///< to this end time. Returns TimeInterval::zero() on
		///< underflow or TimeInterval::limit() on overflow.

	SystemTime operator-( const TimeInterval & interval ) const ;
		///< Returns the time with the given interval offset.

	TimeInterval interval( const SystemTime & end ) const ;
		///< Returns the positive time interval between this
		///< start time and the given later end time. Returns
		///< TimeInterval::zero() on underflow or
		///< TimeInterval::limit() on overflow.

	void streamOut( std::ostream & ) const ;
		///< Streams out the time comprised of the s() value, a
		///< decimal point, and then the six-digit us() value.

private:
	friend class G::DateTime::TimeInterval ;
	friend class G::DateTime::Test ;
	using duration_type = time_point_type::duration ;
	explicit SystemTime( time_point_type ) ;
	SystemTime & add( unsigned long us ) ;

private:
	time_point_type m_tp ;
} ;

//| \class G::DateTime::TimerTime
/// A monotonically increasing subsecond-resolution timestamp, notionally
/// unrelated to time_t.
///
class G::DateTime::TimerTime
{
public:
	using time_point_type = std::chrono::time_point<std::chrono::steady_clock> ;
	using duration_type = time_point_type::duration ;
	static constexpr bool now_noexcept =
		noexcept(time_point_type::clock::now()) &&
		std::is_nothrow_assignable<time_point_type,time_point_type>::value &&
		std::is_nothrow_copy_constructible<time_point_type>::value &&
		noexcept( std::declval<time_point_type>() == std::declval<time_point_type>() ) && // NOLINT(misc-redundant-expression)
		noexcept( std::declval<time_point_type>() += duration_type(1) ) ;
	static constexpr bool less_noexcept = noexcept(time_point_type() < time_point_type()) ; // NOLINT(bogus cert-err58-cpp)

	static TimerTime now() noexcept(now_noexcept) ;
		///< Factory function for the current steady-clock time.

	static TimerTime zero() ;
		///< Factory function for the start of the epoch, guaranteed
		///< to be less than any now().

	bool isZero() const noexcept ;
		///< Returns true if zero().

	bool sameSecond( const TimerTime & other ) const ;
		///< Returns true if this time and the other time are the same,
		///< at second resolution.

	static bool less( const TimerTime & , const TimerTime & ) noexcept(less_noexcept) ;
		///< Comparison operator.

	bool operator<=( const TimerTime & ) const ;
		///< Comparison operator.

	bool operator==( const TimerTime & ) const ;
		///< Comparison operator.

	bool operator!=( const TimerTime & ) const ;
		///< Comparison operator.

	bool operator>( const TimerTime & ) const ;
		///< Comparison operator.

	bool operator>=( const TimerTime & ) const ;
		///< Comparison operator.

	TimerTime operator+( const TimeInterval & ) const ;
		///< Returns this time with given interval added.

	void operator+=( TimeInterval ) ;
		///< Adds an interval.

	TimeInterval operator-( const TimerTime & start ) const ;
		///< Returns the given start time's interval() compared
		///< to this end time. Returns TimeInterval::zero() on
		///< underflow or TimeInterval::limit() on overflow.

	TimeInterval interval( const TimerTime & end ) const ;
		///< Returns the interval between this time and the given
		///< end time. Returns TimeInterval::zero() on underflow
		///< or TimeInterval::limit() on overflow.

private:
	friend class G::DateTime::TimeInterval ;
	friend class G::DateTime::Test ;
	explicit TimerTime( time_point_type ) ;
	static TimerTime test( int , int ) ;
	unsigned long s() const ; // Test
	unsigned long us() const ; // Test
	std::string str() const ; // Test

private:
	time_point_type m_tp ;
} ;

//| \class G::DateTime::TimeInterval
/// A time interval class. Underflows are mapped to the zero()
/// interval and overflows are mapped to limit().
///
class G::DateTime::TimeInterval
{
public:
	using s_type = unsigned int ;
	using us_type = unsigned int ;

	explicit TimeInterval( unsigned int s ) noexcept ;
		///< Constructor for a number of seconds.

	TimeInterval( unsigned int s , unsigned int us ) noexcept ;
		///< Constructor for a number of seconds and microseconds.
		///< The number of microseconds can be more than one million.

	TimeInterval( const SystemTime & start , const SystemTime & end ) ;
		///< Constructor for the interval between two system times.

	TimeInterval( const TimerTime & start , const TimerTime & end ) ;
		///< Constructor for the interval between two timer times.

	static TimeInterval zero() noexcept ;
		///< Factory function for the zero interval.

	static TimeInterval limit() noexcept ;
		///< Factory function for the maximum valid interval.

	static TimeInterval ms( unsigned int ) noexcept ;
		///< Factory function for an interval defined in terms of milliseconds.

	enum class Units : int
	{
		milliseconds = -1 ,
		none = 0 ,
		seconds = 1 ,
		minutes = 60 ,
		hours = 3600 ,
		days = 24*3600
	} ;

	static std::pair<std::string_view,Units> parseUnits( std::string_view ) noexcept ;
		///< Parses a string like "10ms" or "7d" into a numeric
		///< substring and a units enum (so "99s" returns "99"
		///< and Units::seconds). Returns the original string with
		///< Units::none if there is no recognised suffix (in which
		///< case also check with Str::isNumeric()).

	static std::pair<TimeInterval,bool> parse( std::string_view , std::nothrow_t ) noexcept ;
		///< Parses a string like "10ms" or "7d" into an interval,
		///< with true if valid. Returns zero() with false if an
		///< invalid string. Returns limit() with true on overflow.

	static TimeInterval parse( std::string_view , bool throw_on_overflow = false ) ;
		///< Parses a string like "10ms" or "7d" into an interval.
		///< Throws if empty or invalid. Optionally also throws
		///< on overflow.

	unsigned int s() const noexcept ;
		///< Returns the number of seconds.

	unsigned int us() const noexcept ;
		///< Returns the fractional microseconds part.

	void streamOut( std::ostream & ) const ;
		///< Streams out the interval.

	explicit operator bool() const noexcept ;
		///< Returns false iff equal to zero().

	bool operator<( const TimeInterval & ) const noexcept ;
		///< Comparison operator.

	bool operator<=( const TimeInterval & ) const noexcept ;
		///< Comparison operator.

	bool operator==( const TimeInterval & ) const noexcept ;
		///< Comparison operator.

	bool operator!=( const TimeInterval & ) const noexcept ;
		///< Comparison operator.

	bool operator>( const TimeInterval & ) const noexcept ;
		///< Comparison operator.

	bool operator>=( const TimeInterval & ) const noexcept ;
		///< Comparison operator.

	TimeInterval operator+( const TimeInterval & ) const noexcept ;
		///< Returns the combined interval. Returns limit() on overflow.

	TimeInterval operator-( const TimeInterval & ) const noexcept ;
		///< Returns the interval difference. Returns zero() on underflow.

	void operator+=( TimeInterval ) noexcept ;
		///< Adds the given interval. Becomes limit() on overflow.

	void operator-=( TimeInterval ) noexcept ;
		///< Subtracts the given interval. Becomes zero() on underflow.

private:
	using Pair = std::pair<s_type,us_type> ;
	explicit TimeInterval( Pair ) noexcept ;
	static Pair limitPair() noexcept ;
	static Pair makePair( s_type , us_type ) noexcept ;
	template <typename Tp> Pair makePairFromTimepoints( Tp start , Tp end ) ;
	static Pair normalise( Pair ) noexcept ;
	static bool checkedAdd( s_type & s , unsigned int ds ) noexcept ;
	static bool checkedSubtract( s_type & s , unsigned int ds ) noexcept ;
	static TimeInterval parseImp( std::string_view , Units , bool * = nullptr ) noexcept ;

private:
	s_type m_s ;
	us_type m_us ;
} ;

//| \class G::DateTime::Zone
/// A static class that knows about timezone offsets.
///
class G::DateTime::Zone
{
public:
	using Offset = std::pair<bool,unsigned int> ;

	static Offset offset( SystemTime ) ;
		///< Returns the offset in seconds between UTC and localtime
		///< as at the given system time. The returned pair has 'first'
		///< set to true if localtime is ahead of (ie. east of) UTC.

	static std::string offsetString( Offset offset ) ;
		///< Converts the given utc/localtime offset into a five-character
		///< "+/-hhmm" string.
		///< See also RFC-2822.

	static std::string offsetString( int tz ) ;
		///< Overload for a signed integer timezone.

public:
	Zone() = delete ;
} ;

namespace G
{
	namespace DateTime
	{
		std::ostream & operator<<( std::ostream & , const SystemTime & ) ;
		std::ostream & operator<<( std::ostream & , const TimeInterval & ) ;
		inline bool operator<( const TimerTime & a , const TimerTime & b ) noexcept(TimerTime::less_noexcept)
		{
			return TimerTime::less( a , b ) ;
		}
	}
}

constexpr G::DateTime::BrokenDownTime::BrokenDownTime() noexcept :
	m_tm{}
{
	///< m_tm.tm_isdst = -1 ; // not c++11 constexpr, but set to -1 before mktime()
}

inline bool G::DateTime::TimerTime::less( const TimerTime & a , const TimerTime & b ) noexcept(less_noexcept)
{
	return a.m_tp < b.m_tp ;
}

inline bool G::DateTime::BrokenDownTime::operator==( const BrokenDownTime & other ) const noexcept
{
	return sameMinute(other) && m_tm.tm_sec == other.m_tm.tm_sec ;
}

inline bool G::DateTime::BrokenDownTime::operator!=( const BrokenDownTime & other ) const noexcept
{
	return !sameMinute(other) || m_tm.tm_sec != other.m_tm.tm_sec ;
}

constexpr G::DateTime::BrokenDownTime G::DateTime::BrokenDownTime::null() noexcept
{
	return {} ;
}

constexpr bool G::DateTime::BrokenDownTime::valid( int n , int lo , int hi ) noexcept
{
	return n >= lo && n <= hi ;
}

constexpr int G::DateTime::BrokenDownTime::monthDays( int y , int m ) noexcept
{
	return ( m == 1 || m == 3 || m == 5 || m == 7 || m == 8 || m == 10 || m == 12 ) ? 31 :
		( m == 2 ? ( ( ( ((y & 3) == 0) && !((y % 100) == 0) ) || ((y % 400) == 0) ) ? 29 : 28 ) : 30 ) ;
}

constexpr int G::DateTime::BrokenDownTime::monthDays( const std::tm & tm ) noexcept
{
	static_assert( BrokenDownTime::monthDays( 1996 , 2 ) == 29 , "" ) ;
	static_assert( BrokenDownTime::monthDays( 2000 , 2 ) == 29 , "" ) ;
	static_assert( BrokenDownTime::monthDays( 2100 , 2 ) == 28 , "" ) ;
	static_assert( BrokenDownTime::monthDays( 2004 , 2 ) == 29 , "" ) ;
	static_assert( BrokenDownTime::monthDays( 2005 , 2 ) == 28 , "" ) ;
	static_assert( BrokenDownTime::monthDays( 2005 , 1 ) == 31 , "" ) ;
	static_assert( BrokenDownTime::monthDays( 2005 , 11 ) == 30 , "" ) ;
	return monthDays( 1900+tm.tm_year , tm.tm_mon+1 ) ;
}

constexpr bool G::DateTime::BrokenDownTime::valid() const noexcept
{
	return
		valid( m_tm.tm_sec , 0 , 60 ) &&
		valid( m_tm.tm_min , 0 , 59 ) &&
		valid( m_tm.tm_hour , 0 , 23 ) &&
		valid( m_tm.tm_year , 0 , 1000 ) &&
		valid( m_tm.tm_mon , 0 , 11 ) &&
		valid( m_tm.tm_mday , 1 , monthDays(m_tm) ) ;
}

#endif
