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
// gprocess_win32.cpp
//

#include "gdef.h"
#include "gprocess.h"
#include "glog.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <process.h>
#include <direct.h>
#include <io.h>

namespace G
{
	const int STDERR_FILENO = 2 ;
	const int SC_OPEN_MAX = 256 ; // 32 in limits.h !?
} ;

// ===

class G::Process::IdImp
{
public:
	unsigned int m_pid ;
} ;

// ===

G::Process::Id::Id() : m_imp(NULL)
{
	m_imp = new IdImp ;
	m_imp->m_pid = static_cast<unsigned int>(::_getpid()) ; // or ::GetCurrentProcessId()
}

G::Process::Id::~Id()
{
	delete m_imp ;
}

G::Process::Id::Id( const Id & other ) :
	m_imp(NULL)
{
	m_imp = new IdImp ;
	m_imp->m_pid = other.m_imp->m_pid ;
}

G::Process::Id & G::Process::Id::operator=( const Id & rhs )
{
	m_imp->m_pid = rhs.m_imp->m_pid ;
	return *this ;
}

std::string G::Process::Id::str() const
{
	std::stringstream ss ;
	ss << m_imp->m_pid ;
	return ss.str() ;
}

bool G::Process::Id::operator==( const Id & rhs ) const
{
	return m_imp->m_pid == rhs.m_imp->m_pid ;
}

void G::Process::closeFiles( bool keep_stderr )
{
	const int n = SC_OPEN_MAX ;
	for( int fd = 0 ; fd < n ; fd++ )
	{
		if( !keep_stderr || fd != STDERR_FILENO )
			::_close( fd ) ;
	}
}

void G::Process::setUmask()
{
	// _umask() is available but not very useful
	; // no-op
}

void G::Process::closeStderr()
{
	int fd = STDERR_FILENO ;
	::_close( fd ) ;
}

void G::Process::cd( const Path & dir )
{
	if( !cd(dir,NoThrow()) )
		throw CannotChangeDirectory( dir.str() ) ;
}

bool G::Process::cd( const Path & dir , NoThrow )
{
	return 0 == ::_chdir( dir.str().c_str() ) ;
}

int G::Process::spawn( const Path & exe , const std::string & arg ,
	int error_return )
{
	// open file descriptors are inherited across ::_spawn() --
	// no fcntl() is available to set close-on-exec -- but see
	// also ::CreateProcess()

	const char * argv [3U] ;
	argv[0U] = exe.pathCstr() ;
	argv[1U] = arg.c_str() ;
	argv[2U] = NULL ;

	const int mode = _P_WAIT ;
	::_flushall() ;
	G_LOG( "G::Process::spawn: " << exe << " " << arg ) ;
	int rc = ::_spawnv( mode , exe.str().c_str() , argv ) ;
	G_LOG( "G::Process::spawn: done (" << rc << ")" ) ;
	return rc < 0 ? error_return : rc ;
}

bool G::Process::privileged()
{
	return false ;
}

// not implemented...
// int G::Process::errno_()
// Who G::Process::fork() {}
// Who G::Process::fork( Id & child ) {}
// void G::Process::exec( const Path & exe , const std::string & arg ) {}
// int G::Process::wait( const Id & child ) {}
// int G::Process::wait( const Id & child , int error_return ) {}

