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
// gdate.h
//

#ifndef G_DATE_H
#define G_DATE_H

#include "gdef.h"
#include "gdatetime.h"
#include "gdebug.h"
#include <sys/types.h>
#include <time.h>
#include <string>

namespace G
{
	class Date ;
} ;

// Class: G::Date
// Description: A date (dd/mm/yyyy) class.
// See also: Time, DateTime
//
class G::Date
{
public:
	class LocalTime // An overload discriminator class for Date constructors.
		{} ;

	enum Weekday
		{ sunday, monday, tuesday, wednesday, thursday, friday, saturday } ;

	enum Month
		{ january = 1 , february , march , april , may , june , july ,
		august , september , october , november , december } ;

	enum Format
		{ yyyy_mm_dd_slash , yyyy_mm_dd } ;

	static int yearUpperLimit() ;
		// Returns the smallest supported year value.

	static int yearLowerLimit() ;
		// Returns the largest supported year value.

	Date() ;
		// Default constructor the current date
		// in the UTC timezone.

	explicit Date( const LocalTime & ) ;
		// Constructor for the current date
		// in the local timezone.

	Date( const G::DateTime::BrokenDownTime & tm ) ;
		// Constructor for the specified date.

	Date( G::DateTime::EpochTime t , const LocalTime & ) ;
		// Constructor for the date in the local
		// timezone as at the given epoch time.

	Date( int year , Month month , int day_of_month ) ;
		// Constructor for the specified date.

	std::string string( Format format = yyyy_mm_dd_slash ) const ;
		// Returns a string representation of the date.

	Weekday weekday() const ;
		// Returns the day of the week.

	std::string weekdayString( bool brief = false ) const ;
		// Returns an english string representation of
		// the day of the week.

	int monthday() const ;
		// Returns the day of the month.

	std::string monthdayString() const ;
		// Returns a string representation of the day of the month.

	Month month() const ;
		// Returns the month.

	std::string monthString( bool brief = false ) const ;
		// Returns the month as a string (in english).

	int year() const ;
		// Returns the year.

	std::string yearString() const ;
		// Returns the year as a string.

	Date & operator++() ;
		// Increments the date by one day.

	Date & operator--() ;
		// Decrements the date by one day.

	bool operator==( const Date & rhs ) const ;
		// Comparison operator.

	bool operator!=( const Date & rhs ) const ;
		// Comparison operator.

private:
	void init( const G::DateTime::BrokenDownTime & ) ;
	static unsigned int lastDay( unsigned int month , unsigned int year ) ;
	static bool isLeapYear( unsigned int y ) ;

private:
	unsigned int m_day ;
	unsigned int m_month ;
	unsigned int m_year ;
	bool m_weekday_set ;
	Weekday m_weekday ;
} ;

#endif
