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
// gprocess.h
//

#ifndef G_PROCESS_H
#define G_PROCESS_H

#include "gdef.h"
#include "gexception.h"
#include "gpath.h"
#include <iostream>
#include <sys/types.h>
#include <string>

namespace G
{
	class Process ;
} ;

// Class: G::Process
// Description: A static interface for doing things with processes.
// See also: G::Daemon
//
class G::Process
{
public:
	G_EXCEPTION( CannotFork , "cannot fork()" ) ;
	G_EXCEPTION( CannotChangeDirectory , "cannot cd()" ) ;
	G_EXCEPTION( WaitError , "cannot wait()" ) ;
	G_EXCEPTION( ChildError , "child process terminated abnormally or stopped" ) ;
	G_EXCEPTION( InvalidPath , "invalid executable path -- must be absolute" ) ;

	enum Who { Parent , Child } ;
	class IdImp ;
	class Id // Process-id class.
	{
		public: Id() ;
		public: ~Id() ;
		public: Id( const Id & other ) ;
		public: Id & operator=( const Id & rhs ) ;
		public: bool operator==( const Id & other ) const ;
		public: std::string str() const ;
		private: IdImp * m_imp ;
		friend class Process ;
	} ;
	class NoThrow // An overload discriminator for Process.
		{} ;

	static void closeFiles( bool keep_stderr = false ) ;
		// Closes all open file descriptors.

	static void closeStderr() ;
		// Closes stderr.

	static void setUmask() ;
		// Sets a tight umask.

	static void cd( const Path & dir ) ;
		// Changes directory.

	static bool cd( const Path & dir , NoThrow ) ;
		// Changes directory. Returns false on
		// error.

	static Who fork() ;
		// Forks a new process.

	static Who fork( Id & child ) ;
		// Forks a new process. In the parent process
		// the child process-id is returned by reference.

	static void exec( const Path & exe , const std::string & arg = std::string() ) ;
		// Executes a program taking reasonable security
		// precautions.

	static int wait( const Id & child ) ;
		// Waits for a child process to terminate.
		// Returns the exit code. Throws exceptions
		// on error.

	static int wait( const Id & child , int error_return ) ;
		// Waits for a child process to terminate.
		// Returns the exit code, or returns 'error_return'
		// on error.

	static int spawn( const Path & exe , const std::string & arg , int error_return = 127 ) ;
		// Runs a command in a child process. Returns the
		// child process's exit code, or 'error_return' on error.

	static bool privileged() ;
		// Returns true if this process has enhanced security
		// privileges.

private:
	Process() ;
	static int errno_() ;
	static void execCore( const Path & , const std::string & ) ;
} ;

namespace G
{
	inline
	std::ostream & operator<<( std::ostream & stream , const G::Process::Id & id )
	{
		return stream << id.str() ;
	}
} ;

#endif

