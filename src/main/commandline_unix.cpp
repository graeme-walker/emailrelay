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
// commandline_unix.cpp
//

#include "gdef.h"
#include "gsmtp.h"
#include "commandline.h"
#include "gstr.h"
#include <iostream>

Main::CommandLine::Show * Main::CommandLine::Show::m_this = NULL ;

//static
std::string Main::CommandLine::osSwitchSpec()
{
	std::stringstream ss ;
	ss
		<< "l!log!writes log information on standard error (if open) and syslog (if not disabled)!0!|"
		<< "t!no-daemon!does not detach from the terminal!0!|"
		<< "n!no-syslog!disables syslog output!0!|"
		<< "q!as-client!equivalent to \"--log --no-syslog --no-daemon --dont-serve --forward --forward-to\"!" << "1!host:port|"
		<< "d!as-server!equivalent to \"--log --close-stderr\"!0!"
		;
	return ss.str() ;
}

Main::CommandLine::Show::Show( bool e ) :
	m_e(e)
{
}

std::ostream & Main::CommandLine::Show::s()
{
	return m_e ? std::cerr : std::cout ;
}

Main::CommandLine::Show::~Show()
{
}

unsigned int Main::CommandLine::ttyColumns() const
{
	const unsigned int default_ = 79U ;
	try
	{
		const char * p = std::getenv( "COLUMNS" ) ;
		if( p == NULL )
			return default_ ;
		else
			return G::Str::toUInt(p) ;
	}
	catch( std::exception & )
	{
		return default_ ;
	}
}

