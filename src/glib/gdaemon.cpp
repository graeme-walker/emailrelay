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
// gdaemon.cpp
//

#include "gdef.h"
#include "gdaemon.h"
#include "gprocess.h"

//static
void G::Daemon::PidFile::check( const G::Path & pid_file )
{
	if( pid_file != G::Path() && !pid_file.isAbsolute() )
		throw G::Daemon::BadPidFile(std::string("must be an absolute path: ")+pid_file.str()) ;
}

//static
void G::Daemon::PidFile::test( const G::Path & pid_file )
{
	if( pid_file != G::Path() )
	{
		std::ofstream tester( pid_file.str().c_str() ) ;
		if( !tester.good() )
			throw G::Daemon::BadPidFile(std::string("cannot create file: ")+pid_file.str()) ;
	}
}

//static
void G::Daemon::PidFile::create( const G::Path & pid_file )
{
	if( pid_file != G::Path() )
	{
		std::ofstream file( pid_file.str().c_str() ) ;
		file << G::Process::Id() << std::endl ;
		if( !file.good() )
			throw G::Daemon::BadPidFile(std::string("cannot create file: ")+pid_file.str()) ;
	}
}

G::Daemon::PidFile::PidFile() :
	m_valid(false)
{
}

G::Daemon::PidFile::PidFile( const G::Path & path ) :
	m_path(path) ,
	m_valid(true)
{
}

void G::Daemon::PidFile::commit()
{
	if( m_valid )
		create( m_path ) ;
}


