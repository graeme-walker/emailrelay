//
// Copyright (C) 2001-2003 Graeme Walker <graeme_walker@users.sourceforge.net>
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

// Class: Main::CommandLine::ShowImp
// Description: A private implementation class used by Main::CommandLine::Show.
//
class Main::CommandLine::Show::Imp
{
public:
	bool m_e ;
	explicit Imp( bool b ) : m_e(b) {}
} ;

//static
std::string Main::CommandLine::osSwitchSpec()
{
	std::ostringstream ss ;
	ss
		<< "l!log!writes log information on standard error and syslog!0!!2|"
		<< "t!no-daemon!does not detach from the terminal!0!!3|"
		<< "u!user!names the effective user to switch to when started as root (default is \"daemon\")!1!username!3|"
		<< "n!no-syslog!disables syslog output!0!!3"
		;
	return ss.str() ;
}

Main::CommandLine::Show::Show( bool e ) :
	m_imp( new Imp(e) )
{
}

std::ostream & Main::CommandLine::Show::s()
{
	return m_imp->m_e ? std::cerr : std::cout ;
}

Main::CommandLine::Show::~Show()
{
	delete m_imp ;
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

