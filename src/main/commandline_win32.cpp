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
// commandline_win32.cpp
//

#include "gdef.h"
#include "gsmtp.h"
#include "commandline.h"

Main::CommandLine::Show * Main::CommandLine::Show::m_this = NULL ;

//static
std::string Main::CommandLine::osSwitchSpec()
{
	// (could use empty descriptions for some switches so that they
	// do not appear in the "--help" listing, but that might be
	// confusing)

	std::stringstream ss ;
	ss
		<< "l!log!writes log information on standard error (if open)!0!|"
		<< "t!no-daemon!use an ordinary window, not the system tray!0!|"
		<< "n!no-syslog!dont use the event log!0!|"
		<< "I!icon!selects the application icon!1!0^|1^|2^|3"
		;

	return ss.str() ;
}

unsigned int Main::CommandLine::ttyColumns() const
{
	return 120U ;
}

// ===

Main::CommandLine::Show::Show( bool )
{
	if( m_this == NULL )
	{
		m_this = this ;
	}
}

std::ostream & Main::CommandLine::Show::s()
{
	return m_this->m_ss ;
}

Main::CommandLine::Show::~Show()
{
	if( m_this == this )
	{
		m_this = NULL ;
		::MessageBox( NULL , m_ss.str().c_str() , "E-MailRelay" , MB_OK ) ;
	}
}

