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
// gprocess_unix.cpp
//

#include "gdef.h"
#include "gprocess.h"
#include "gfs.h"
#include "glog.h"
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h> // open()
#include <pwd.h> // getpwnam()
#include <unistd.h> // setuid() etc

// Class: G::Process::IdImp
// Description: A private implementation class used by G::Process.
//
class G::Process::IdImp
{
public:
	pid_t m_pid ;
} ;

// ===

//static
void G::Process::cd( const Path & dir )
{
	if( ! cd(dir,NoThrow()) )
		throw CannotChangeDirectory( dir.str() ) ;
}

//static
bool G::Process::cd( const Path & dir , NoThrow )
{
	return 0 == ::chdir( dir.str().c_str() ) ;
}

//static
void G::Process::setUmask()
{
	mode_t new_mode = 0177 ; // create as -rw-------
	(void) ::umask( new_mode ) ;
}

//static
void G::Process::closeStderr()
{
	::close( STDERR_FILENO ) ;
	::open( G::FileSystem::nullDevice() , O_WRONLY ) ;
	::fcntl( STDERR_FILENO , F_SETFD , 0 ) ; // close-on-exec false
}

//static
void G::Process::closeFiles( bool keep_stderr )
{
	int n = 256U ;
	long rc = ::sysconf( _SC_OPEN_MAX ) ;
	if( rc > 0L )
		n = static_cast<int>( rc ) ;

	for( int fd = 0 ; fd < n ; fd++ )
	{
		if( !keep_stderr || fd != STDERR_FILENO )
		{
			::close( fd ) ;
		}
	}

	// reopen standard fds to prevent accidental use
	// of arbitrary files or sockets as standard
	// streams
	//
	::open( G::FileSystem::nullDevice() , O_RDONLY ) ;
	::open( G::FileSystem::nullDevice() , O_WRONLY ) ;
	if( !keep_stderr )
	{
		::open( G::FileSystem::nullDevice() , O_WRONLY ) ;
		::fcntl( STDERR_FILENO , F_SETFD , 0 ) ; // close-on-exec false
	}
	::fcntl( STDIN_FILENO , F_SETFD , 0 ) ; // close-on-exec false
	::fcntl( STDOUT_FILENO , F_SETFD , 0 ) ; // close-on-exec false
}

G::Process::Who G::Process::fork()
{
	Id id ;
	return fork( id ) ;
}

G::Process::Who G::Process::fork( Id & child_pid )
{
	pid_t rc = ::fork() ;
	const bool ok = rc != -1 ;
	if( ok )
	{
		if( rc != 0 )
			child_pid.m_imp->m_pid = rc ;
	}
	else
	{
		throw CannotFork() ;
	}
	return rc == 0 ? Child : Parent ;
}

int G::Process::wait( const Id & child_pid )
{
	int status ;
	for(;;)
	{
		G_DEBUG( "G::Process::wait: waiting" ) ;
		int rc = ::waitpid( child_pid.m_imp->m_pid , &status , 0 ) ;
		if( rc == -1 && errno_() == EINTR )
		{
			; // signal in parent -- keep waiting
		}
		else if( rc == -1 )
		{
			int error = errno_() ;
			throw WaitError( std::stringstream() << "errno=" << error ) ;
		}
		else
		{
			break ;
		}
	}
	G_DEBUG( "G::Process::wait: done" ) ;

	if( ! WIFEXITED(status) )
	{
		// uncaught signal or stopped
		throw ChildError( std::stringstream() << "status=" << status ) ;
	}

	const int exit_status = WEXITSTATUS(status) ;
	return exit_status ;
}

int G::Process::wait( const Id & child_pid , int error_return )
{
	try
	{
		return wait( child_pid ) ;
	}
	catch(...)
	{
	}
	return error_return ;
}

//static
int G::Process::errno_()
{
	return errno ; // not ::errno or std::errno for gcc2.95
}

void G::Process::exec( const G::Path & exe , const std::string & arg )
{
	if( exe.isRelative() )
		throw InvalidPath( exe.str() ) ;

	if( privileged() )
		throw Insecure() ;

	closeFiles() ;

	execCore( exe , arg ) ;
}

void G::Process::execCore( const G::Path & exe , const std::string & arg )
{
	char * argv[3U] ;
	argv[0U] = const_cast<char*>( exe.pathCstr() ) ;
	argv[1U] = arg.empty() ? static_cast<char*>(NULL) : const_cast<char*>(arg.c_str()) ;
	argv[2U] = NULL ;

	char * env[3U] ;
	std::string path( "PATH=/usr/bin:/bin" ) ; // no "."
	std::string ifr( "IFR= \t\n" ) ;
	env[0U] = const_cast<char*>( path.c_str() ) ;
	env[1U] = const_cast<char*>( ifr.c_str() ) ;
	env[2U] = NULL ;

	::execve( exe.str().c_str() , argv , env ) ;

	const int error = errno_() ;
	G_WARNING( "G::Process::exec: execve() returned: errno=" << error << ": " << exe ) ;
}

int G::Process::spawn( const G::Path & exe , const std::string & arg , int error_return )
{
	if( exe.isRelative() )
		throw InvalidPath( exe.str() ) ;

	if( privileged() )
		throw Insecure() ;

	Id child_pid ;
	if( fork(child_pid) == Child )
	{
		exec( exe , arg ) ;
		::_exit( error_return ) ;
	}
	else
	{
		return wait( child_pid , error_return ) ;
	}
}

bool G::Process::privileged()
{
	return ::getuid() == 0U || ::geteuid() == 0U ;
}

void G::Process::beSpecial( Identity identity )
{
	// try to change our effective id -- this
	// will only work if our real uid is root, or if
	// the executable is suid (assuming the
	// o/s supports the saved-suid feature)
	//
	// gcc requires -D_BSD_SOURCE for seteuid()
	//
	Identity old_identity ;
	(void) ::seteuid( identity.uid ) ;
	(void) ::setegid( identity.gid ) ;
	(void) old_identity.str() ; // pacify the compiler
	G_DEBUG( "G::Process::beSpecial: " << old_identity << " -> " << Identity() ) ;
}

G::Process::Identity G::Process::beOrdinary( Identity nobody )
{
	Identity special_identity ;
	if( ::getuid() == 0 )
	{
		if( ::seteuid(0) ) throw UidError("0") ; // first
		if( ::setegid(nobody.gid) ) throw GidError(nobody.str()) ; // second
		if( ::seteuid(nobody.uid) ) throw UidError(nobody.str()) ; // third
	}
	else
	{
		// switch our effective id back to our real id --
		// ie. turn off the effects of a suid executable
		if( ::seteuid( ::getuid() ) ) throw UidError() ;
		if( ::setegid( ::getgid() ) ) throw GidError() ;
	}
	G_DEBUG( "G::Process::beOrdinary: " << special_identity << " -> " << Identity() ) ;
	return special_identity ;
}

// ===

G::Process::Id::Id() : m_imp(NULL)
{
	m_imp = new IdImp ;
	m_imp->m_pid = ::getpid() ;
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

// ===

G::Process::Identity::Identity() :
	uid(::geteuid()) ,
	gid(::getegid())
{
}

G::Process::Identity::Identity( const std::string & name ) :
	uid(static_cast<uid_t>(-1)) ,
	gid(static_cast<gid_t>(-1))
{
	if( ::getuid() == 0 )
	{
		::passwd * pw = ::getpwnam( name.c_str() ) ;
		if( pw == NULL )
			throw Process::NoSuchUser(name) ;
		G_DEBUG( "G::Process::Identity: " << name << "=" << pw->pw_uid << "/" << pw->pw_gid ) ;
		uid = pw->pw_uid ;
		gid = pw->pw_gid ;
	}
}

std::string G::Process::Identity::str() const
{
	std::stringstream ss ;
	ss << uid << "/" << gid ;
	return ss.str() ;
}

