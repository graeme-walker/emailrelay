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
// commandline_win32.cpp
//

#include "gdef.h"
#include "gsmtp.h"
#include "commandline.h"

Main::CommandLine::Show * Main::CommandLine::Show::m_this = NULL ;

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

unsigned int Main::CommandLine::ttyColumns() const
{
	return 120U ;
}

