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
// gdaemon.h
//

#ifndef G_DAEMON_H
#define G_DAEMON_H

#include "gdef.h"
#include "gexception.h"
#include "gpath.h"
#include <sys/types.h>
#include <string>

namespace G
{
	class Daemon ;
} ;

// Class: G::Daemon
// Description: A class for deamonising the calling process.
// Deamonisation includes fork()ing, detaching from the
// controlling terminal, setting the process umask, etc.
// The windows implementation does nothing.
// See also: G::Process
//
class G::Daemon
{
public:
	G_EXCEPTION( CannotFork , "cannot fork" ) ;
	G_EXCEPTION( BadPidFile , "invalid pid file" ) ;
	class PidFile // Used by G::Daemon::detach().
	{
		public: explicit PidFile( const Path & pid_file ) ;
		public: PidFile() ;
		public: void commit() ;
		private: Path m_path ;
		private: bool m_valid ;
		friend class Daemon ;
		private: static void check( const Path & ) ;
		private: static void test( const Path & ) ;
		private: static void create( const Path & ) ;
	} ;

	static void detach() ;
		// Detaches from the parent environment.
		// This typically involves fork()ing,
		// _exit()ing the parent, and calling
		// setsid() in the child.

	static void detach( const Path & pid_file ) ;
		// An overload which writes the new process-id
		// to a file. The path must be absolute.
		// Throws BadPidFile on error.

	static void detach( PidFile & pid_file ) ;
		// An overload which allows for a delayed write
		// of the new process-id to a file. The path
		// must be absolute.
		//
		// A delayed write is useful for network daemons
		// which open a listening port. You do not want
		// a second instance, which will fail on startup,
		// to overwrite the pid file of the running
		// server. In this situation call PidFile::commit()
		// just before entering the event loop.
		//
		// Throws BadPidFile on error.

private:
	Daemon() ;
	static void setsid() ;
} ;

#endif

