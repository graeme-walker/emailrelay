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
// glogoutput_win32.cpp
//

#include "gdef.h"
#include "glogoutput.h"
#include <cstdlib> // getenv

void G::LogOutput::rawOutput( G::Log::Severity severity , const char *message )
{
	std::cerr << message ;
	std::cerr.flush() ;

	static bool debugger = std::getenv("GLOGOUTPUT_DEBUGGER") != NULL ;
	if( debugger )
	{
		::OutputDebugString( message ) ;
	}

	static const char * key = "GLOGOUTPUT_FILE" ;
	static const char * filename = std::getenv( key ) ;
	if( filename != NULL && *filename != '\0' )
	{
		static std::ofstream file( filename ) ;
		file << message ;
		file.flush() ;
	}
}

void G::LogOutput::syslog()
{
	// no-op
	// see also ::RegisterEventSource() and ::ReportEvent() for NT
}

