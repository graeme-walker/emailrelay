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
// gdaemon_unix.cpp
//

#include "gdef.h"
#include "gdaemon.h"
#include "gprocess.h"

//static
void G::Daemon::detach( const Path & pid_file )
{
	PidFile::check( pid_file ) ;
	PidFile::test( pid_file ) ;

	detach() ;

	PidFile::create( pid_file ) ;
}

//static
void G::Daemon::detach( PidFile & pid_file )
{
	PidFile::check( pid_file.m_path ) ;
	detach() ;
}

//static
void G::Daemon::detach()
{
	// see Stevens, ISBN 0-201-563137-7, ch 13.

	if( Process::fork() == Process::Parent )
		::_exit( 0 ) ;

	setsid() ;
	(void) Process::cd( "/" , Process::NoThrow() ) ;

	if( Process::fork() == Process::Parent )
		::_exit( 0 ) ;
}

void G::Daemon::setsid()
{
	pid_t session_id = ::setsid() ;
	if( session_id == -1 )
		; // no-op
}

