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
/// \file gdatetime.cpp
///

#include "gdef.h"
#include "gdatetime.h"
#include "goptional.h"
#include "gstr.h"
#include "gstringvalue.h"
#include "gstringview.h"
#include "gassert.h"
#include <sstream>
#include <iomanip>
#include <utility>
#include <vector>
#include <limits>
#include <type_traits>

namespace G
{
	namespace DateTimeImp
	{
		static constexpr std::string_view good_format( "%ntYyCGgmUWVjdwuHIMSDFRT" , 24U ) ;
		static_assert( good_format[good_format.size()-1U] == 'T' , "" ) ;
		static constexpr DateTime::TimeInterval::us_type million = 1000000U ;

		bool next( int & n , int lo , int hi ) noexcept // NOLINT(misc-use-internal-linkage)
		{
			n = n >= hi ? lo : (n+1) ;
			return n == lo ;
		}
		bool previous( int & n , int lo , int hi ) noexcept // NOLINT(misc-use-internal-linkage)
		{
			n = n <= lo ? hi : (n-1) ;
			return n == hi ;
		}
		static bool less( const std::tm & a , const std::tm & b ) noexcept
		{
			if( a.tm_year != b.tm_year ) return a.tm_year < b.tm_year ;
			if( a.tm_mon != b.tm_mon ) return a.tm_mon < b.tm_mon ;
			if( a.tm_mday != b.tm_mday ) return a.tm_mday < b.tm_mday ;
			if( a.tm_hour != b.tm_hour ) return a.tm_hour < b.tm_hour ;
			if( a.tm_min != b.tm_min ) return a.tm_min < b.tm_min ;
			return a.tm_sec < b.tm_sec ;
		}
		static bool sameMinute( const std::tm & a , const std::tm & b ) noexcept
		{
			return
				a.tm_year == b.tm_year &&
				a.tm_mon == b.tm_mon &&
				a.tm_mday == b.tm_mday &&
				a.tm_hour == b.tm_hour &&
				a.tm_min == b.tm_min ;
		}
		static bool sameSecond( const std::tm & a , const std::tm & b ) noexcept
		{
			return sameMinute( a , b ) && a.tm_sec == b.tm_sec ;
		}
		static void localtime_( std::tm & tm_out , std::time_t t_in )
		{
			if( localtime_r( &t_in , &tm_out ) == nullptr )
				throw DateTime::Error() ;
			tm_out.tm_isdst = -1 ;
		}
		static void gmtime_( std::tm & tm_out , std::time_t t_in )
		{
			if( gmtime_r( &t_in , &tm_out ) == nullptr )
				throw DateTime::Error() ;
			tm_out.tm_isdst = -1 ;
		}
		static std::time_t mktime_( std::tm & tm )
		{
			tm.tm_isdst = -1 ;
			std::time_t t = std::mktime( &tm ) ;
			if( t == std::time_t(-1) )
				throw DateTime::Error() ;
			return t ;
		}
		static std::time_t mktimelocal( const std::tm & local_tm_in )
		{
			struct std::tm tm = local_tm_in ;
			return mktime_( tm ) ;
		}
		static std::time_t mktimeutc( const std::tm & utc_tm_in , std::time_t begin , std::time_t end )
		{
			// returns 't' such that std::gmtime(t) gives the target broken-down
			// time -- does a binary search over the given time_t range, down
			// to one second resolution
			std::time_t count = end - begin ;
			std::time_t t = begin ;
			while( count > 0 )
			{
				std::time_t i = t ;
				std::time_t step = count / 2 ;
				i += step ;
				std::tm tm {} ;
				gmtime_( tm , i ) ;
				if( less( tm , utc_tm_in ) )
				{
					t = ++i ;
					count -= step + 1 ;
				}
				else
				{
					count = step ;
				}
			}
			return t ;
		}
	}
}

G::DateTime::BrokenDownTime::BrokenDownTime( const struct std::tm & tm_in ) :
	m_tm(tm_in)
{
	// dont trust the dst flag passed in -- force mktime()
	// to do the extra work (strftime() does anyway)
	m_tm.tm_isdst = -1 ;
}

G::DateTime::BrokenDownTime::BrokenDownTime( int y , int mon , int d , int h , int min , int s ) :
	m_tm{}
{
	m_tm.tm_year = y - 1900 ;
	m_tm.tm_mon = mon - 1 ;
	m_tm.tm_mday = d ;
	m_tm.tm_hour = h ;
	m_tm.tm_min = min ;
	m_tm.tm_sec = s ;
	m_tm.tm_isdst = -1 ;
	m_tm.tm_wday = 0 ;
	m_tm.tm_yday = 0 ;
}

std::time_t G::DateTime::BrokenDownTime::epochTimeFromLocal() const
{
	return valid() ? DateTimeImp::mktimelocal( m_tm ) : std::time_t(0) ;
}

std::time_t G::DateTime::BrokenDownTime::epochTimeFromUtc() const
{
	if( !valid() )
		return std::time_t(0) ;

	std::time_t t0 = DateTimeImp::mktimelocal( m_tm ) ;

	// mktimeutc() does multiple gmtime()s to determine the timezone
	// offset, so prefer one gmtime() with a memoised offset
	static std::optional<std::time_t> memo ;
	if( memo.has_value() )
	{
		std::tm tm {} ;
		DateTimeImp::gmtime_( tm , t0+memo.value() ) ;
		if( DateTimeImp::sameSecond(tm,m_tm) )
			return t0 + memo.value() ;
	}

	std::time_t dt = 25 * 3600 + 10 ;
	std::time_t begin = std::max(dt,t0) - dt ;
	std::time_t end = t0 + dt ;
	std::time_t t = DateTimeImp::mktimeutc( m_tm , begin , end ) ;
	if( t == begin || t == end )
		throw DateTime::Error( "timezone error" ) ;

	memo = t - t0 ;
	return t ;
}

G::DateTime::BrokenDownTime G::DateTime::BrokenDownTime::local( SystemTime t )
{
	BrokenDownTime bdt ;
	bdt.m_tm.tm_isdst = -1 ;
	DateTimeImp::localtime_( bdt.m_tm , t.s() ) ;
	return bdt ;
}

G::DateTime::BrokenDownTime G::DateTime::BrokenDownTime::utc( SystemTime t )
{
	BrokenDownTime bdt ;
	bdt.m_tm.tm_isdst = -1 ;
	DateTimeImp::gmtime_( bdt.m_tm , t.s() ) ;
	return bdt ;
}

G::DateTime::BrokenDownTime G::DateTime::BrokenDownTime::midday( int year , int month , int day )
{
	return { year , month , day , 12 , 0 , 0 } ;
}

G::DateTime::BrokenDownTime G::DateTime::BrokenDownTime::midnight( int year , int month , int day )
{
	return { year , month , day , 0 , 0 , 0 } ;
}

bool G::DateTime::BrokenDownTime::format( char * out , std::size_t out_size , const char * fmt ) const
{
	for( const char * p = std::strchr(fmt,'%') ; p && p[1] ; p = std::strchr(p+1,'%') )
	{
		if( DateTimeImp::good_format.find(p[1]) == std::string::npos )
			throw DateTime::Error( "bad format string" ) ;
	}

	std::tm tm_copy = m_tm ;
	DateTimeImp::mktime_( tm_copy ) ; // fill in isdst, wday, yday

	return std::strftime( out , out_size , fmt , &tm_copy ) > 0U ;
}

void G::DateTime::BrokenDownTime::format( std::vector<char> & out , const char * fmt ) const
{
	if( !format( out.data() , out.size() , fmt ) )
		throw DateTime::Error() ;
}

std::string G::DateTime::BrokenDownTime::str() const
{
	return str( "%F %T" ) ;
}

std::string G::DateTime::BrokenDownTime::str( const char * fmt ) const
{
	std::size_t n = std::strlen( fmt ) + 1U ;
	for( const char * p = std::strchr(fmt,'%') ; p && p[1] ; p = std::strchr(p+1,'%') )
		n += 10U ; // biggest allowed format is eg. %F -> "2001-12-31"

	std::vector<char> buffer( n ) ;
	format( buffer , fmt ) ;
	buffer.at(buffer.size()-1U) = '\0' ; // just in case
	return { buffer.data() } ;
}

G::DateTime::BrokenDownTime G::DateTime::BrokenDownTime::parse( const std::string & s )
{
	if( s.size() >= 19U )
	{
		int yyyy = Str::toInt( s.substr(0U,4U) , "-1" ) ;
		int mon = Str::toInt( s.substr(5U,2U) , "-1" ) ;
		int dd = Str::toInt( s.substr(8U,2U) , "-1" ) ;
		int hh = Str::toInt( s.substr(11U,2U) , "-1" ) ;
		int min = Str::toInt( s.substr(14U,2U) , "-1" ) ;
		int ss = Str::toInt( s.substr(17U,2U) , "-1" ) ;
		if( yyyy >= 0 && mon >= 0 && dd >= 0 && hh >= 0 && min >= 0 && ss >= 0 )
			return { yyyy , mon , dd , hh , min , ss } ;
	}
	static_assert( !null().valid() , "" ) ;
	return null() ;
}

int G::DateTime::BrokenDownTime::hour() const
{
	return m_tm.tm_hour ;
}

int G::DateTime::BrokenDownTime::min() const
{
	return m_tm.tm_min ;
}

int G::DateTime::BrokenDownTime::sec() const
{
	return m_tm.tm_sec ;
}

int G::DateTime::BrokenDownTime::year() const
{
	return m_tm.tm_year + 1900 ;
}

int G::DateTime::BrokenDownTime::month() const
{
	return m_tm.tm_mon + 1 ;
}

int G::DateTime::BrokenDownTime::day() const
{
	return m_tm.tm_mday ;
}

int G::DateTime::BrokenDownTime::wday() const
{
	std::tm tm_copy = m_tm ;
	DateTimeImp::mktime_( tm_copy ) ;
	return tm_copy.tm_wday ;
}

bool G::DateTime::BrokenDownTime::sameMinute( const BrokenDownTime & other ) const noexcept
{
	return DateTimeImp::sameMinute( m_tm , other.m_tm ) ;
}

bool G::DateTime::BrokenDownTime::operator<( const BrokenDownTime & other ) const noexcept
{
	return DateTimeImp::less( m_tm , other.m_tm ) ;
}

G::DateTime::BrokenDownTime G::DateTime::BrokenDownTime::next( Unit unit ) const
{
	namespace imp = DateTimeImp ;
	auto tm = m_tm ;
	bool carry = false ;
	if( unit == Unit::minute )
		carry = imp::next( tm.tm_min , 0 , 59 ) ;
	if( carry || unit == Unit::hour )
		carry = imp::next( tm.tm_hour , 0 , 23 ) ;
	if( carry || unit == Unit::day )
		carry = imp::next( tm.tm_mday , 1 , monthDays(tm) ) , imp::next( tm.tm_wday , 0 , 6 ) ;
	if( carry || unit == Unit::month )
		carry = imp::next( tm.tm_mon , 0 , 11 ) ;
	if( carry || unit == Unit::year )
		tm.tm_year++ ;
	return BrokenDownTime( tm ) ;
}

G::DateTime::BrokenDownTime G::DateTime::BrokenDownTime::previous( Unit unit ) const
{
	namespace imp = DateTimeImp ;
	auto tm = m_tm ;
	bool borrow = false ;
	if( unit == Unit::minute )
		borrow = imp::previous( tm.tm_min , 0 , 59 ) ;
	if( borrow || unit == Unit::hour )
		borrow = imp::previous( tm.tm_hour , 0 , 23 ) ;
	if( borrow || unit == Unit::day )
		borrow = imp::previous( tm.tm_mday , 1 , monthDays(tm) ) , imp::previous( tm.tm_wday , 0 , 6 ) ;
	if( borrow || unit == Unit::month )
		borrow = imp::previous( tm.tm_mon , 0 , 11 ) ;
	if( borrow || unit == Unit::year )
		tm.tm_year++ ;
	return BrokenDownTime( tm ) ;
}

// ==

G::DateTime::SystemTime::SystemTime( time_point_type tp ) :
	m_tp(tp)
{
}

G::DateTime::SystemTime::SystemTime( std::time_t t , unsigned long us ) noexcept
{
	m_tp = std::chrono::system_clock::from_time_t(t) ;
	m_tp += std::chrono::microseconds( us ) ;
}

G::DateTime::SystemTime G::DateTime::SystemTime::now() noexcept(now_noexcept)
{
	return SystemTime( std::chrono::system_clock::now() ) ;
}

G::DateTime::SystemTime G::DateTime::SystemTime::operator-( const TimeInterval & interval ) const
{
	auto tp = m_tp ;
	tp -= std::chrono::seconds( interval.s() ) ;
	tp -= std::chrono::microseconds( interval.us() ) ;
	return SystemTime( tp ) ;
}

G::DateTime::TimeInterval G::DateTime::SystemTime::operator-( const SystemTime & start ) const
{
	return { start , *this } ;
}

G::DateTime::TimeInterval G::DateTime::SystemTime::interval( const SystemTime & end ) const
{
	return { *this , end } ;
}

G::DateTime::SystemTime & G::DateTime::SystemTime::add( unsigned long us )
{
	m_tp += std::chrono::microseconds( us ) ;
	return *this ;
}

bool G::DateTime::SystemTime::sameSecond( const SystemTime & other ) const noexcept
{
	return s() == other.s() ;
}

G::DateTime::BrokenDownTime G::DateTime::SystemTime::local() const
{
	return BrokenDownTime::local( *this ) ;
}

G::DateTime::BrokenDownTime G::DateTime::SystemTime::utc() const
{
	return BrokenDownTime::utc( *this ) ;
}

unsigned int G::DateTime::SystemTime::ms() const
{
	using namespace std::chrono ;
	return static_cast<unsigned int>((duration_cast<milliseconds>(m_tp.time_since_epoch()) % seconds(1)).count()) ;
}

unsigned int G::DateTime::SystemTime::us() const
{
	using namespace std::chrono ;
	return static_cast<unsigned int>((duration_cast<microseconds>(m_tp.time_since_epoch()) % seconds(1)).count()) ;
}

std::time_t G::DateTime::SystemTime::s() const
{
	using namespace std::chrono ;
	G_ASSERT( system_clock::to_time_t(time_point_type()) == 0 ) ; // assert time_point_type uses time_t's 1970 epoch (as per c++17)
	// cannot use system_clock::to_time_t() here because the implementation might do rounding
	return static_cast<std::time_t>((duration_cast<seconds>(m_tp.time_since_epoch())).count()) ;
}

G::DateTime::SystemTime G::DateTime::SystemTime::zero()
{
	using namespace std::chrono ;
	return SystemTime( time_point_type() ) ;
}

bool G::DateTime::SystemTime::isZero() const
{
	return m_tp == time_point_type( duration_type(0) ) ;
}

bool G::DateTime::SystemTime::operator<( const SystemTime & other ) const
{
	return m_tp < other.m_tp ;
}

bool G::DateTime::SystemTime::operator<=( const SystemTime & other ) const
{
	return m_tp <= other.m_tp ;
}

bool G::DateTime::SystemTime::operator==( const SystemTime & other ) const
{
	return m_tp == other.m_tp ;
}

bool G::DateTime::SystemTime::operator!=( const SystemTime & other ) const
{
	return !( *this == other ) ;
}

bool G::DateTime::SystemTime::operator>( const SystemTime & other ) const
{
	return m_tp > other.m_tp ;
}

bool G::DateTime::SystemTime::operator>=( const SystemTime & other ) const
{
	return m_tp >= other.m_tp ;
}

G::DateTime::SystemTime G::DateTime::SystemTime::operator+( TimeInterval interval ) const
{
	SystemTime t( *this ) ;
	t += interval ;
	return t ;
}

void G::DateTime::SystemTime::operator+=( TimeInterval i )
{
	using namespace std::chrono ;
	m_tp += seconds(i.s()) ;
	m_tp += microseconds(i.us()) ;
}

void G::DateTime::SystemTime::operator-=( TimeInterval i )
{
	using namespace std::chrono ;
	m_tp -= seconds(i.s()) ;
	m_tp -= microseconds(i.us()) ;
}

void G::DateTime::SystemTime::streamOut( std::ostream & stream ) const
{
	int w = static_cast<int>( stream.width() ) ;
	char c = stream.fill() ;
	stream
		<< s() << "."
		<< std::setw(6) << std::setfill('0')
		<< us()
		<< std::setw(w) << std::setfill(c) ;
}

std::ostream & G::DateTime::operator<<( std::ostream & stream , const SystemTime & t )
{
	t.streamOut( stream ) ;
	return stream ;
}

// ==

G::DateTime::TimerTime::TimerTime( time_point_type tp ) :
	m_tp(tp)
{
}

G::DateTime::TimerTime G::DateTime::TimerTime::now() noexcept(now_noexcept)
{
	time_point_type tp = std::chrono::steady_clock::now() ;
	if( tp == time_point_type() ) tp += duration_type(1) ; // postcondition >zero()
	return TimerTime( tp ) ;
}

G::DateTime::TimerTime G::DateTime::TimerTime::zero()
{
	return TimerTime( time_point_type() ) ;
}

bool G::DateTime::TimerTime::isZero() const noexcept
{
	return m_tp == time_point_type( duration_type(0) ) ;
}

G::DateTime::TimerTime G::DateTime::TimerTime::test( int s , int us )
{
	using namespace std::chrono ;
	return TimerTime( time_point_type( seconds(s) + microseconds(us) ) ) ;
}

unsigned long G::DateTime::TimerTime::s() const
{
	using namespace std::chrono ;
	return static_cast<unsigned long>( duration_cast<seconds>(m_tp.time_since_epoch()).count() ) ;
}

unsigned long G::DateTime::TimerTime::us() const
{
	using namespace std::chrono ;
	return static_cast<unsigned long>( (duration_cast<microseconds>(m_tp.time_since_epoch()) % seconds(1)).count() ) ;
}

std::string G::DateTime::TimerTime::str() const
{
	std::ostringstream ss ;
	ss << s() << '.' << std::setw(6) << std::setfill('0') << us() ;
	return ss.str() ;
}

G::DateTime::TimerTime G::DateTime::TimerTime::operator+( const TimeInterval & interval ) const
{
	TimerTime t( *this ) ;
	t += interval ;
	return t ;
}

void G::DateTime::TimerTime::operator+=( TimeInterval i )
{
	using namespace std::chrono ;
	m_tp += seconds(i.s()) ;
	m_tp += microseconds(i.us()) ;
}

G::DateTime::TimeInterval G::DateTime::TimerTime::operator-( const TimerTime & start ) const
{
	return { start , *this } ;
}

G::DateTime::TimeInterval G::DateTime::TimerTime::interval( const TimerTime & end ) const
{
	return { *this , end } ;
}

bool G::DateTime::TimerTime::sameSecond( const TimerTime & other ) const
{
	using namespace std::chrono ;
	return
		duration_cast<seconds>(m_tp.time_since_epoch()) ==
		duration_cast<seconds>(other.m_tp.time_since_epoch()) ;
}

bool G::DateTime::TimerTime::operator<=( const TimerTime & other ) const
{
	return m_tp <= other.m_tp ;
}

bool G::DateTime::TimerTime::operator==( const TimerTime & other ) const
{
	return m_tp == other.m_tp ;
}

bool G::DateTime::TimerTime::operator!=( const TimerTime & other ) const
{
	return m_tp != other.m_tp ;
}

bool G::DateTime::TimerTime::operator>( const TimerTime & other ) const
{
	return m_tp > other.m_tp ;
}

bool G::DateTime::TimerTime::operator>=( const TimerTime & other ) const
{
	return m_tp >= other.m_tp ;
}

// ==

G::DateTime::TimeInterval::TimeInterval( Pair pair ) noexcept : // private ctor
	m_s(pair.first) ,
	m_us(pair.second)
{
}

G::DateTime::TimeInterval::TimeInterval( unsigned int s ) noexcept :
	TimeInterval(normalise(makePair(s,0U)))
{
}

G::DateTime::TimeInterval::TimeInterval( unsigned int s , unsigned int us ) noexcept :
	TimeInterval(normalise(makePair(s,us)))
{
}

G::DateTime::TimeInterval::TimeInterval( const SystemTime & start , const SystemTime & end ) :
	TimeInterval(makePairFromTimepoints(start.m_tp,end.m_tp))
{
}

G::DateTime::TimeInterval::TimeInterval( const TimerTime & start , const TimerTime & end ) :
	TimeInterval(makePairFromTimepoints(start.m_tp,end.m_tp))
{
}

G::DateTime::TimeInterval::Pair G::DateTime::TimeInterval::makePair( unsigned int s , unsigned int us ) noexcept
{
	// assert no overflow shenanigans required here
	static_assert( std::is_same<decltype(s),s_type>::value , "" ) ;
	static_assert( std::is_same<decltype(us),us_type>::value , "" ) ;
	return { s , us } ;
}

template <typename Tp>
G::DateTime::TimeInterval::Pair G::DateTime::TimeInterval::makePairFromTimepoints( Tp start , Tp end )
{
	using namespace std::chrono ;
	namespace imp = DateTimeImp ;

	// calculate a duration from timepoint range
	if( end <= start )
		return {0U,0U} ;
	auto duration = end - start ;

	// split duration into seconds and microseconds
	auto duration_s = (duration_cast<seconds>(duration)).count() ;
	auto duration_us = (duration_cast<microseconds>(duration) % seconds(1)).count() ;
	static_assert( std::is_integral<decltype(duration_s)>::value , "" ) ;
	static_assert( std::is_integral<decltype(duration_us)>::value , "" ) ;

	// sanity checks
	G_ASSERT( duration_s >= 0 ) ;
	G_ASSERT( duration_us >= 0 && duration_us < imp::million ) ;
	if( duration_s < 0 || duration_us < 0 || duration_us >= imp::million )
		return {0U,0U} ; // never gets here

	// limit on overflow
	{
		using U = typename std::make_unsigned<decltype(duration_s)>::type ;
		auto unsigned_duration_s = static_cast<U>( duration_s ) ;
		if( unsigned_duration_s > std::numeric_limits<s_type>::max() )
			return limitPair() ;
	}

	// pre-checked narrowing casts
	s_type s = static_cast<s_type>(duration_s) ;
	us_type us = static_cast<us_type>(duration_us) ;

	return { s , us } ;
}

G::DateTime::TimeInterval::Pair G::DateTime::TimeInterval::normalise( Pair pair ) noexcept
{
	namespace imp = DateTimeImp ;
	s_type & s = pair.first ;
	us_type & us = pair.second ;
	if( us >= imp::million )
	{
		us -= imp::million ;
		if( checkedAdd( s , 1U ) )
			return limitPair() ;

		if( us >= imp::million ) // still
		{
			s_type ds = us / imp::million ;
			us = ( us % imp::million ) ;
			if( checkedAdd( s , ds ) )
				return limitPair() ;
		}
	}
	return pair ;
}

G::DateTime::TimeInterval::Pair G::DateTime::TimeInterval::limitPair() noexcept
{
	namespace imp = DateTimeImp ;
	return { std::numeric_limits<s_type>::max() , imp::million-1U } ;
}

G::DateTime::TimeInterval G::DateTime::TimeInterval::limit() noexcept
{
	return TimeInterval( limitPair() ) ;
}

G::DateTime::TimeInterval G::DateTime::TimeInterval::zero() noexcept
{
	return TimeInterval( Pair{0U,0U} ) ;
}

std::pair<std::string_view,G::DateTime::TimeInterval::Units> G::DateTime::TimeInterval::parseUnits( std::string_view value ) noexcept
{
	if( Str::tailMatch( value , "ms" ) )
		return { sv_substr_noexcept( value , 0U , value.size()-2U ) , Units::milliseconds } ;
	else if( Str::tailMatch( value , "s" ) )
		return { sv_substr_noexcept( value , 0U , value.size()-1U ) , Units::seconds } ;
	else if( Str::tailMatch( value , "m" ) )
		return { sv_substr_noexcept( value , 0U , value.size()-1U ) , Units::minutes } ;
	else if( Str::tailMatch( value , "h" ) )
		return { sv_substr_noexcept( value , 0U , value.size()-1U ) , Units::hours } ;
	else if( Str::tailMatch( value , "d" ) )
		return { sv_substr_noexcept( value , 0U , value.size()-1U ) , Units::days } ;
	return { value , Units::none } ;
}

std::pair<G::DateTime::TimeInterval,bool> G::DateTime::TimeInterval::parse( std::string_view value , std::nothrow_t ) noexcept
{
	auto pair = parseUnits( value ) ;
	if( pair.first.empty() || !Str::isNumeric(pair.first) )
		return { zero() , false } ;
	else
		return { parseImp(pair.first,pair.second) , true } ;
}

G::DateTime::TimeInterval G::DateTime::TimeInterval::parse( std::string_view value , bool throw_on_overflow )
{
	auto pair = parseUnits( value ) ;
	if( pair.first.empty() || !Str::isNumeric(pair.first) )
		throw DateTime::Error( "invalid interval string" ) ;
	bool overflow = false ;
	auto result = parseImp( pair.first , pair.second , &overflow ) ;
	if( overflow && throw_on_overflow )
		throw DateTime::Error( "interval overflow" ) ;
	return result ;
}

G::DateTime::TimeInterval G::DateTime::TimeInterval::parseImp( std::string_view value , Units units , bool * overflow_p ) noexcept
{
	G_ASSERT( !value.empty() && Str::isNumeric(value) ) ;
	bool invalid = false ;
	bool overflow = false ;
	TimeInterval result = zero() ;
	if( units == Units::milliseconds )
	{
		us_type us = StringValue::parse<us_type>( value , overflow , invalid , 1000U ) ;
		G_ASSERT( !overflow || value.size() > 3U ) ;
		if( overflow && value.size() > 3U )
			result = TimeInterval( StringValue::parse<s_type>( sv_substr_noexcept(value,0U,value.size()-3U) , overflow , invalid ) ) ;
		else
			result = TimeInterval( 0U , us ) ; // inc. normalise()
		if( overflow )
			result = limit() ;
	}
	else
	{
		unsigned int scale = units == Units::none ? 1U : static_cast<unsigned>(units) ;
		s_type s = StringValue::parse<s_type>( value , overflow , invalid , scale ) ;
		result = overflow ? limit() : TimeInterval(s) ;
	}
	G_ASSERT( !invalid ) ; // pre-checked by caller
	if( invalid )
		result = zero() ; // never gets here
	if( overflow_p )
		*overflow_p = overflow ;
	return result ;
}

G::DateTime::TimeInterval G::DateTime::TimeInterval::ms( unsigned int ms ) noexcept
{
	if( ms >= 1000U )
		return { ms/1000U , (ms%1000U)*1000U } ;
	else
		return { 0UL , ms * 1000U } ;
}

G::DateTime::TimeInterval::s_type G::DateTime::TimeInterval::s() const noexcept
{
	return m_s ;
}

G::DateTime::TimeInterval::us_type G::DateTime::TimeInterval::us() const noexcept
{
	return m_us ;
}

G::DateTime::TimeInterval::operator bool() const noexcept
{
	return m_s || m_us ;
}

bool G::DateTime::TimeInterval::operator==( const TimeInterval & other ) const noexcept
{
	return m_s == other.m_s && m_us == other.m_us ;
}

bool G::DateTime::TimeInterval::operator!=( const TimeInterval & other ) const noexcept
{
	return !( *this == other ) ;
}

bool G::DateTime::TimeInterval::operator<( const TimeInterval & other ) const noexcept
{
	return m_s < other.m_s || ( m_s == other.m_s && m_us < other.m_us ) ;
}

bool G::DateTime::TimeInterval::operator<=( const TimeInterval & other ) const noexcept
{
	return *this == other || *this < other ;
}

bool G::DateTime::TimeInterval::operator>( const TimeInterval & other ) const noexcept
{
	return m_s > other.m_s || ( m_s == other.m_s && m_us > other.m_us ) ;
}

bool G::DateTime::TimeInterval::operator>=( const TimeInterval & other ) const noexcept
{
	return *this == other || *this > other ;
}

G::DateTime::TimeInterval G::DateTime::TimeInterval::operator+( const TimeInterval & other ) const noexcept
{
	TimeInterval t( *this ) ;
	t += other ;
	return t ;
}

G::DateTime::TimeInterval G::DateTime::TimeInterval::operator-( const TimeInterval & other ) const noexcept
{
	TimeInterval t( *this ) ;
	t -= other ;
	return t ;
}

void G::DateTime::TimeInterval::operator+=( TimeInterval i ) noexcept
{
	namespace imp = DateTimeImp ;
	bool overflow = false ;
	m_us += i.m_us ;
	if( m_us >= imp::million )
	{
		m_us -= imp::million ;
		if( checkedAdd(m_s,1U) )
			overflow = true ;
	}
	if( checkedAdd( m_s , i.m_s ) )
		overflow = true ;
	if( overflow )
	{
		m_s = limitPair().first ;
		m_us = limitPair().second ;
	}
}

void G::DateTime::TimeInterval::operator-=( TimeInterval i ) noexcept
{
	namespace imp = DateTimeImp ;
	bool underflow = false ;
	if( m_us < i.m_us )
	{
		if( checkedSubtract(m_s,1U) )
			underflow = true ;
		m_us += imp::million ;
	}
	m_us -= i.m_us ;
	if( checkedSubtract(m_s,i.m_s) )
		underflow = true ;
	if( underflow )
	{
		m_s = 0U ;
		m_us = 0U ;
	}
}

bool G::DateTime::TimeInterval::checkedAdd( s_type & s , unsigned int ds ) noexcept
{
	const auto old = s ;
	s += ds ;
	return s < old ; // overflow
}

bool G::DateTime::TimeInterval::checkedSubtract( s_type & s , unsigned int ds ) noexcept
{
	if( ds > s )
		return true ; // underflow
	s -= ds ;
	return false ;
}

void G::DateTime::TimeInterval::streamOut( std::ostream & stream ) const
{
	int w = static_cast<int>( stream.width() ) ;
	char c = stream.fill() ;
	stream
		<< s() << "."
		<< std::setw(6) << std::setfill('0')
		<< us()
		<< std::setw(w) << std::setfill(c) ;
}

std::ostream & G::DateTime::operator<<( std::ostream & stream , const TimeInterval & ti )
{
	ti.streamOut( stream ) ;
	return stream ;
}

// ==

G::DateTime::Zone::Offset G::DateTime::Zone::offset( SystemTime t_in )
{
	G_ASSERT( !(t_in == SystemTime::zero()) ) ;
	SystemTime t_zone( BrokenDownTime::local(t_in).epochTimeFromUtc() ) ;
	bool ahead = t_in < t_zone ; // ie. east-of
	TimeInterval i = ahead ? (t_zone-t_in) : (t_in-t_zone) ;
	return Offset{ ahead , i.s() } ;
}

std::string G::DateTime::Zone::offsetString( int tz )
{
	std::ostringstream ss ;
	ss << ( tz < 0 ? "-" : "+" ) ;
	if( tz < 0 ) tz = -tz ;
	ss << (tz/10) << (tz%10) << "00" ;
	return ss.str() ;
}

std::string G::DateTime::Zone::offsetString( Offset offset )
{
	unsigned int hh = (offset.second+30U) / 3600U ;
	unsigned int mm = ((offset.second+30U) / 60U) % 60 ;

	std::ostringstream ss ;
	char sign = (offset.first || (hh==0&&mm==0)) ? '+' : '-' ;
	ss << sign << (hh/10U) << (hh%10U) << (mm/10) << (mm%10) ;
	return ss.str() ;
}
