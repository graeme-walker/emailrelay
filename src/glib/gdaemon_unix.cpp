//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gdaemon_unix.cpp
///

#include "gdef.h"
#include "gdaemon.h"
#include "gprocess.h"
#include "gfile.h"
#include "gnewprocess.h"
#include <chrono>
#include <thread>

namespace G
{
	namespace DaemonImp
	{
		void waitfor( const G::Path & pid_file )
		{
			if( !pid_file.empty() )
			{
				for( int i = 0 ; i < 100 ; i++ )
				{
					if( G::File::exists( pid_file , std::nothrow ) )
						break ;
					std::this_thread::sleep_for( std::chrono::milliseconds(10) ) ;
				}
			}
		}
	}
}

void G::Daemon::detach()
{
	detach( G::Path() ) ;
}

void G::Daemon::detach( const G::Path & pid_file )
{
	// see Stevens, ISBN 0-201-563137-7, ch 13.

	if( !NewProcess::fork().first )
	{
		DaemonImp::waitfor( pid_file ) ; // because systemd
		std::_Exit( 0 ) ; // exit from parent
	}

	setsid() ;
	Process::cd( "/" , std::nothrow ) ;

	if( !NewProcess::fork().first )
		std::_Exit( 0 ) ; // exit from parent
}

void G::Daemon::setsid()
{
	GDEF_IGNORE_RETURN ::setsid() ;
}

