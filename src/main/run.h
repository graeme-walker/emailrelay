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
// run.h
//

#ifndef G_MAIN_RUN_H
#define G_MAIN_RUN_H

#include "gdef.h"
#include "gsmtp.h"
#include "configuration.h"
#include "commandline.h"
#include "gevent.h"
#include "gdaemon.h"
#include "garg.h"
#include "gmessagestore.h"
#include "gsmtpclient.h"
#include <iostream>
#include <exception>
#include <memory>

namespace Main
{
	class Run ;
} ;

// Class: Main::Run
// Description: A top-level class for the process.
// Usage:
/// int main( int argc , char ** argv )
/// {
///   G::Arg arg( argc , argv ) ;
///   Main::Run run( arg ) ;
///   if( run.prepare() )
///      run.run() ;
///   return 0 ;
/// }
//
class Main::Run : private GSmtp::Client::ClientCallback
{
public:
	explicit Run( const G::Arg & arg ) ;
		// Constructor.

	bool prepare() ;
		// Prepares to run(). Returns
		// false on error.

	void run() ;
		// Runs the application.
		// Precondition: prepare() returned true

	Configuration cfg() const ;
		// Returns a configuration object.

	static std::string versionNumber() ;
		// Returns the application version number string.

private:
	void runCore() ;
	void doForwarding( GSmtp::MessageStore & , GNet::EventSources & ) ;
	void doServing( G::Daemon::PidFile & , GNet::EventSources & ) ;
	void closeFiles() ;
	void closeMoreFiles() ;
	std::string smtpIdent() const ;
	void recordPid() ;
	const CommandLine & cl() const ;
	virtual void clientDone( std::string ) ;

private:
	std::auto_ptr<CommandLine> m_cl ;
	G::Arg m_arg ;
} ;

#endif
