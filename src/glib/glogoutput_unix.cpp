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
// glogoutput_unix.cpp
//

#include "gdef.h"
#include "glogoutput.h"
#include <syslog.h>
#include <iostream>

namespace
{
	int mode( G::LogOutput::SyslogFacility facility , G::Log::Severity severity )
	{
		int m = 0 ;

		if( facility == G::LogOutput::User ) m |= LOG_USER ;
		else if( facility == G::LogOutput::Daemon ) m |= LOG_DAEMON ;
		else if( facility == G::LogOutput::Mail ) m |= LOG_MAIL ;
		else if( facility == G::LogOutput::Cron ) m |= LOG_CRON ;
		// etc...

		if( severity == G::Log::s_Warning ) m |= LOG_WARNING ;
		else if( severity == G::Log::s_Error ) m |= LOG_ERR ;
		else if( severity == G::Log::s_Log ) m |= LOG_INFO ;
		else m |= LOG_CRIT ;
	
		return m ;
	}
} ;

void G::LogOutput::rawOutput( G::Log::Severity severity , const char *message )
{
	if( severity != G::Log::s_Debug && m_syslog && !(message[0]=='\n'&&message[1]=='\0') )
	{
		::syslog( mode(m_facility,severity) , "%s" , message ) ;
	}
	std::cerr << message ;
	std::cerr.flush() ;
}

void G::LogOutput::syslog()
{
	syslog( User ) ;
}

