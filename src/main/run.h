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
// run.h
//

#ifndef G_MAIN_RUN_H
#define G_MAIN_RUN_H

#include "gdef.h"
#include "gsmtp.h"
#include "configuration.h"
#include "commandline.h"
#include "geventloop.h"
#include "gtimer.h"
#include "glogoutput.h"
#include "gdaemon.h"
#include "gpidfile.h"
#include "garg.h"
#include "gmessagestore.h"
#include "gsmtpclient.h"
#include <iostream>
#include <exception>
#include <memory>

namespace Main
{
	class Run ;
}

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

	virtual ~Run() ;
		// Destructor.

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

	static bool startForwarding( GSmtp::Client::ClientCallback & ,
		const std::string & to ) ;
			// Starts forwarding spooled email.
			// Should be called from within run().
			// Returns false if there is nothing to
			// do or if nothing can be done. Throws
			// on error. If true is returned then the
			// callback object must remain valid
			// until its clientDone() method is
			// called.

	void raiseStoreEvent() ;
		// A pseudo-private method.

	void raiseNetworkEvent( const std::string & , const std::string & ) ;
		// A pseudo-private method.

protected:
	virtual void onEvent( const std::string & category ,
		const std::string & s1 , const std::string & s2 ) ;
			// Overridable. Called when something changes.

	virtual bool runnable() const ;
		// Overridable. Allows derived classes to have prepare()
		// return false. The default implementation
		// returns true.

private:
	Run( const Run & ) ; // not implemented
	void operator=( const Run & ) ; // not implemented
	void runCore() ;
	void doForwarding( GSmtp::MessageStore & , GNet::EventLoop & ) ;
	void doServing( G::PidFile & , GNet::EventLoop & ) ;
	void closeFiles() ;
	void closeMoreFiles() ;
	std::string smtpIdent() const ;
	void recordPid() ;
	const CommandLine & cl() const ;
	virtual void clientDone( std::string ) ; // from ClientCallback
	virtual void clientEvent( const std::string & , const std::string & ) ; // from ClientCallback
	const char * startForwarding( GSmtp::Client::ClientCallback * , const std::string & ) ;

private:
	static Run * m_this ;
	std::auto_ptr<CommandLine> m_cl ;
	std::auto_ptr<G::LogOutput> m_log_output ;
	std::auto_ptr<GSmtp::Client> m_client ;
	G::Arg m_arg ;
} ;

#endif
