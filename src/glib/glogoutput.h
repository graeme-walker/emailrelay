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
// glogoutput.h
//
	
#ifndef G_LOG_OUTPUT_H
#define G_LOG_OUTPUT_H

#include "gdef.h"
#include "glog.h"

namespace G
{
	class LogOutput ;
} ;

// Class: G::LogOutput
// Description: Controls and implements low-level logging output, as used by the Log interface.
// Applications should normally instantiate a LogOutput object in main() to enable
// log output.
// See also: Log
//
class G::LogOutput
{
public:
 	enum SyslogFacility { User , Daemon , Mail , Cron } ; // etc.

	explicit LogOutput( bool logging_enabled , bool verbose = true ) ;
		// Constructor. If there is no LogOutput object,
		// or if 'logging_enabled' is false, then there is no
		// output of any sort. If both parameters are true
		// then debug messages will be generated in addition
		// to the log/warning/error messages (as long
		// as it was compiled in).
		//
		// More than one LogOutput object may be created, but
		// only the first one controls output.
		
	virtual ~LogOutput() ;
		// Destructor.

	virtual void rawOutput( G::Log::Severity s , const char *string ) ;
		// Overridable. Used to do the final message
		// output (with OutputDebugString() or stderr).
		
	static LogOutput *instance() ;
		// Returns a pointer to the controlling
		// LogOutput object. Returns NULL if none.
		
	bool enable( bool debug_enabled = true ) ;
		// Enables or disables debug output.
		// Returns the previous setting.

	void syslog() ;
		// Enables logging to the syslog system under Unix.

	void timestamp() ;
		// Enables timestamping.

	void syslog( SyslogFacility facility ) ;
		// Enables logging to the syslog system under Unix,
		// using the specified facility.
	
	static void output( G::Log::Severity s , const char *raw_output ) ;
		// Generates debug output if there is an extant
		// LogOutput object which is enabled. Uses rawOutput().

	static void output( G::Log::Severity s , const char *file , unsigned line , const char *text ) ;
		// Generates debug output if there is an extant
		// LogOutput object which is enabled. Uses rawOutput().

	static void assertion( const char *file , unsigned line , bool test , const char *test_string ) ;	
		// Makes an assertion check (regardless of any LogOutput
		// object). Calls output() if the 'file' parameter is
		// not null.

	virtual void onAssert() ;
		// Called during an assertion failure. This allows
		// Windows applications to stop timers etc. which
		// cause reentrancy problems and infinitely recursive
		// dialog box creation.

private:
	LogOutput( const LogOutput & ) ;
	void operator=( const LogOutput & ) ;
	static void itoa( char *out , unsigned int ) ;
	static void addFileAndLine( char * , size_t , const char * , int ) ;
	static void addTimestamp( char * , size_t , const char * ) ;
	const char * timestampString() ;
	static void halt() ;
	void doOutput( G::Log::Severity , const char * ) ;
	void doOutput( G::Log::Severity s , const char * , unsigned , const char * ) ;

private:
	static LogOutput * m_this ;
	bool m_enabled ;
	bool m_verbose ;
	bool m_syslog ;
	SyslogFacility m_facility ;
	time_t m_time ;
	char m_time_buffer[40U] ;
	bool m_timestamp ;
} ;

#endif
