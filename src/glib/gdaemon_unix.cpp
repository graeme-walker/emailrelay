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
// gdaemon_unix.cpp
//

#include "gdef.h"
#include "gdaemon.h"
#include "gpid.h"

//static
void G::Daemon::detach( const Path & pid_file )
{
	if( !pid_file.isAbsolute() )
		throw BadPidFile(std::string("must be an absolute path: ")+pid_file.str()) ;

	if( !std::ofstream(pid_file.str().c_str()).good() )
		throw BadPidFile(std::string("cannot create file: ")+pid_file.str()) ;

	detach() ;

	std::ofstream file( pid_file.str().c_str() ) ;
	file << Pid() << std::endl ;
}

//static
void G::Daemon::detach()
{
	// see Stevens, ISBN 0-201-563137-7, ch 13.

	if( fork() == Parent )
		::_exit( 0 ) ;

	setsid() ;
	cd( "/" ) ;

	if( fork() == Parent )
		::_exit( 0 ) ;
}

void G::Daemon::setsid()
{
	pid_t session_id = ::setsid() ;
	if( session_id == -1 )
		; // no-op
}

void G::Daemon::cd( const std::string & dir )
{
	if( 0 != ::chdir( dir.c_str() ) )
		; // ignore it
}

void G::Daemon::setUmask()
{
	// (note that ansi std::ofstream does not support file permissions,
	// so rely on the umask to keep things secure)
	mode_t new_mode = 0177 ; // create as -rw-------
	mode_t old_mode = ::umask( new_mode ) ;
}

G::Daemon::Who G::Daemon::fork()
{
	pid_t pid = ::fork() ;
	if( pid < 0 )
	{
		throw CannotFork() ;
	}
	return pid == 0 ? Child : Parent ;
}

void G::Daemon::closeStderr()
{
	::close( STDERR_FILENO ) ;
}

void G::Daemon::closeFiles( bool keep_stderr )
{
	int n = 256U ;
	long rc = ::sysconf( _SC_OPEN_MAX ) ;
	if( rc > 0L )
		n = static_cast<int>( rc ) ;

	for( int fd = 0 ; fd < n ; fd++ )
	{
		if( !keep_stderr || fd != STDERR_FILENO )
			::close( fd ) ;
	}
}

