//
// Copyright (C) 2001-2002 Graeme Walker <graeme_walker@users.sourceforge.net>
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
// glogoutput.cpp
//

#include "gdef.h"
#include "glogoutput.h"
#include <cstdlib>
#include <cstring>
#include <ctime>

G::LogOutput *G::LogOutput::m_this = NULL ;

G::LogOutput::LogOutput( bool enabled , bool verbose ) :
	m_enabled(enabled) ,
	m_verbose(verbose) ,
	m_syslog(false) ,
	m_time(0) ,
	m_timestamp(false)
{
	if( m_this == NULL )
		m_this = this ;
}

G::LogOutput::~LogOutput()
{
	if( m_this == this )
		m_this = NULL ;
}

bool G::LogOutput::enable( bool enabled )
{
	bool was_enabled = m_enabled ;
	m_enabled = enabled ;
	return was_enabled ;
}

//static
void G::LogOutput::itoa( char *out , unsigned int n )
{
	n = n % 1000000U ;

	if( n == 0U )
	{
		out[0U] = '0' ;
		out[1U] = '\0' ;
	}
	else
	{
		char buffer[15U] ;
		char *p = buffer + sizeof(buffer) - 1 ;
		*p-- = '\0' ;

		for( ; n > 0U ; --p )
		{
			*p = '0' + (n % 10U) ;
			n /= 10U ;
		}

		std::strcpy( out , p+1 ) ;
	}
}

void G::LogOutput::timestamp()
{
	m_timestamp = true ;
}

const char * G::LogOutput::timestampString()
{
	std::time_t now = std::time(NULL) ;
	if( m_time == 0 || m_time != now )
	{
		m_time = now ;
		struct std::tm * tm_p = std::localtime( &m_time ) ;
		m_time_buffer[0] = '\0' ;
		std::strftime( m_time_buffer , sizeof(m_time_buffer)-1U , "%Y" "%m" "%d." "%H" "%M" "%S: " , tm_p ) ;
		m_time_buffer[sizeof(m_time_buffer)-1U] = '\0' ;
	}
	return m_time_buffer ;
}

//static
void G::LogOutput::output( G::Log::Severity severity , const char *text )
{
	if( m_this != NULL )
		m_this->doOutput( severity , text ) ;
}

void G::LogOutput::doOutput( G::Log::Severity severity , const char *text )
{
	if( m_enabled )
	{
		if( severity != G::Log::s_Debug || m_verbose )
		{
			rawOutput( severity , text ? text : "" ) ;
			if( text && text[0U] && text[std::strlen(text)-1U] != '\n' )
				rawOutput( severity , "\n" ) ;
		}
	}
}

//static
void G::LogOutput::output( G::Log::Severity severity , const char *file, unsigned line, const char *text )
{
	if( m_this != NULL )
		m_this->doOutput( severity , file , line , text ) ;
}

void G::LogOutput::doOutput( G::Log::Severity severity , const char *file, unsigned line, const char *text )
{
	if( m_enabled )
	{
		file = file ? file : "" ;
		text = text ? text : "" ;

		char buffer[500U] ;
		buffer[0U] = '\0' ;
		if( severity == G::Log::s_Debug )
			addFileAndLine( buffer , sizeof(buffer) , file , line ) ;
		else if( m_timestamp )
			addTimestamp( buffer , sizeof(buffer) , timestampString() ) ;
		std::strncat( buffer + std::strlen(buffer) , text , sizeof(buffer) - 1U - std::strlen(buffer) ) ;
		output( severity , buffer ) ;
	}
}

G::LogOutput *G::LogOutput::instance()
{
	return m_this ;
}

void G::LogOutput::onAssert()
{
	// no-op
}

//static
void G::LogOutput::addFileAndLine( char *buffer , size_t size , const char *file , int line )
{
	const char *forward = std::strrchr( file , '/' ) ;
	const char *back = std::strrchr( file , '\\' ) ;
	const char *last = forward > back ? forward : back ;
	const char *basename = last ? (last+1) : file ;

	std::strncat( buffer+std::strlen(buffer) , basename , size-std::strlen(buffer)-1U ) ;
	std::strncat( buffer+std::strlen(buffer) , "(" , size-std::strlen(buffer)-1U ) ;
	char b[15U] ;
	itoa( b , line ) ; // (implemented above)
	std::strncat( buffer+std::strlen(buffer) , b , size-std::strlen(buffer)-1U ) ;
	std::strncat( buffer+std::strlen(buffer) , "): " , size-std::strlen(buffer)-1U ) ;
}

//static
void G::LogOutput::addTimestamp( char *buffer , size_t size , const char * ts )
{
	std::strncat( buffer+std::strlen(buffer) , ts , size-std::strlen(buffer)-1U ) ;
}

void G::LogOutput::assertion( const char *file , unsigned line , bool test , const char *test_string )
{
	if( !test )
	{
		char buffer[100U] ;
		std::strcpy( buffer , "Assertion error: " ) ;
		size_t size = sizeof(buffer) - 10U ; // -10 for luck
		if( file )
		{
			addFileAndLine( buffer , size , file , line ) ;
		}
		if( test_string )
		{
			std::strncat( buffer+std::strlen(buffer) , test_string , size-std::strlen(buffer)-1U);
		}

		if( instance() )
		{
			// forward to derived classes -- these
			// overrides may safely re-enter this method --
			// all code in this class is re-entrant
			//
			instance()->onAssert() ;
		}

		output( G::Log::s_Assertion , buffer ) ;
		halt() ;
	}
}

//static
void G::LogOutput::halt()
{
	abort() ;
}

void G::LogOutput::syslog( SyslogFacility facility )
{
	m_syslog = true ;
	m_facility = facility ;
}

