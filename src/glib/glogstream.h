//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file glogstream.h
///

#ifndef G_LOG_STREAM_H
#define G_LOG_STREAM_H

#include "gdef.h"
#include <ios>

namespace G
{
	//| \class G::LogStream
	/// A non-throwing moveable wrapper for std::ostream, used by
	/// G::LogOutput and associated logging macros. The most common
	/// streaming operators are implemented out-of-line as a modest
	/// code-size optimisation.
	///
	struct LogStream
	{
		LogStream( unsigned int & depth , std::ostream & ostream ) noexcept :
			m_depth_p(&depth) ,
			m_ostream(&ostream)
		{
			depth++ ;
		}
		~LogStream()
		{
			if( m_depth_p && *m_depth_p )
				(*m_depth_p)-- ;
		}
		LogStream() noexcept = default ;
		LogStream( const LogStream & ) noexcept = delete ;
		LogStream( LogStream && ) noexcept = default ;
		LogStream & operator=( const LogStream & ) noexcept = delete ;
		LogStream & operator=( LogStream && ) noexcept = default ;
		unsigned int * m_depth_p {nullptr} ;
		std::ostream * m_ostream {nullptr} ;
	} ;
}

namespace G
{
	LogStream & operator<<( LogStream & s , const std::string & ) noexcept ;
	LogStream & operator<<( LogStream & s , const char * ) noexcept ;
	LogStream & operator<<( LogStream & s , char ) noexcept ;
	LogStream & operator<<( LogStream & s , unsigned char ) noexcept ;
	LogStream & operator<<( LogStream & s , int ) noexcept ;
	LogStream & operator<<( LogStream & s , unsigned int ) noexcept ;
	LogStream & operator<<( LogStream & s , long ) noexcept ;
	LogStream & operator<<( LogStream & s , unsigned long ) noexcept ;
	LogStream & operator<<( LogStream & s , void * /*handle*/ ) noexcept ;

	template <typename T> LogStream & operator<<( LogStream & s , const T & t ) noexcept
	{
		if( s.m_ostream )
		{
			try
			{
				*(s.m_ostream) << t ;
			}
			catch(...)
			{
			}
		}
		return s ;
	}
}

#endif
