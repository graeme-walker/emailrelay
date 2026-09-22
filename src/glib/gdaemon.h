//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gdaemon.h
///

#ifndef G_DAEMON_H
#define G_DAEMON_H

#include "gdef.h"
#include "gexception.h"
#include "gpidfile.h"
#include "gpath.h"
#include <sys/types.h>
#include <string>

namespace G
{
	class Daemon ;
}

//| \class G::Daemon
/// A static interface for daemonising the calling process. Daemonisation
/// includes fork()ing, detaching from the controlling terminal, setting
/// the process umask, etc. The windows implementation does nothing.
/// \see G::Process
///
class G::Daemon
{
public:
	static void detach() ;
		///< Detaches from the parent environment. This typically
		///< involves fork()ing, std::_Exit()ing the parent, and calling
		///< setsid() in the child. See also G::PidFile.

	static void detach( const G::Path & pid_file ) ;
		///< Does a detach() but the calling process waits a while
		///< for the pid file to be created before it exits.

public:
	Daemon() = delete ;

private:
	static void setsid() ;
} ;

#endif
