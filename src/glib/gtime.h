//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gtime.h
///

#ifndef G_TIME_H
#define G_TIME_H

#include "gdef.h"
#include "gexception.h"
#include "gdatetime.h"
#include <ctime>

namespace G
{
	class Time ;
}

//| \class G::Time
/// A simple time-of-day (hh/mm/ss) class.
/// \see G::Date, G::DateTime
///
class G::Time
{
public:
	class LocalTime /// An overload discriminator class for Time constructors.
		{} ;

	Time() ;
		///< Constructor for the current time, using UTC.

	Time( int hh , int mm , int ss ) ;
		///< Constructor for the given time. Out-of-range values are
		///< clamped, with leap seconds allowed only just before
		///< midnight.

	explicit Time( const BrokenDownTime & tm ) ;
		///< Constructor for the given broken-down time.

	explicit Time( SystemTime t ) ;
		///< Constructor for the given epoch time, using UTC.

	Time( SystemTime t , const LocalTime & ) ;
		///< Constructor for the given epoch time, using the local timezone.

	explicit Time( const LocalTime & ) ;
		///< Constructor for the current time, using the local timezone.

	static Time at( unsigned int ) ;
		///< Factory function for a time that is the given number of
		///< seconds since midnight (see value()). Values that are out
		///< of the normal time-of-day range are clamped, with leap
		///< seconds beging lost in the process.

	int hours() const ;
		///< Returns the hours (0 <= h < 24).

	int minutes() const ;
		///< Returns the minutes (0 <= m < 60).

	int seconds() const ;
		///< Returns the seconds (0 <= s <= 61).

	std::string hhmmss( const char * sep = nullptr ) const ;
		///< Returns the hhmmss string.

	std::string hhmm( const char * sep = nullptr ) const ;
		///< Returns the hhmm string.

	std::string ss() const ;
		///< Returns the seconds as a two-digit decimal string.

	unsigned int value() const ;
		///< Returns the time as the number of seconds since midnight
		///< (ignoring leap seconds).

	bool operator==( const Time & ) const ;
		///< Comparison operator.

	bool operator!=( const Time & ) const ;
		///< Comparison operator.

private:
	static std::string xx( int ) ;

private:
	int m_hh ;
	int m_mm ;
	int m_ss ;
} ;

#endif
