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
// run.cpp
//

#include "gdef.h"
#include "gsmtp.h"
#include "run.h"
#include "gsmtpserver.h"
#include "gsmtpclient.h"
#include "gsasl.h"
#include "gsecrets.h"
#include "geventloop.h"
#include "garg.h"
#include "gdaemon.h"
#include "gpidfile.h"
#include "gfilestore.h"
#include "gnewfile.h"
#include "gadminserver.h"
#include "gmonitor.h"
#include "glocal.h"
#include "groot.h"
#include "gexception.h"
#include "gprocess.h"
#include "gmemory.h"
#include "glogoutput.h"
#include "gdebug.h"
#include <iostream>
#include <exception>

namespace Main
{
	class FileStore ;
	class Monitor ;
}

// Class: Main::FileStore
// Description: A derivation of GSmtp::FileStore which is used to
// do event plumbing into Main::Run.
//
class Main::FileStore : public GSmtp::FileStore
{
	private: Run & m_run ;
	public: FileStore( Run & run , const G::Path & path ) : GSmtp::FileStore(path) , m_run(run) {}
	private: virtual void onEvent() { m_run.raiseStoreEvent() ; }
} ;

// Class: Main::Monitor
// Description: A derivation of GNet::Monitor which is used to do
// event plumbing into Main::Run.
//
class Main::Monitor : public GNet::Monitor
{
	private: Run & m_run ;
	public: Monitor( Run & run ) : m_run(run) {}
	private: virtual void onEvent( const std::string & s1 , const std::string & s2 )
		{ m_run.raiseNetworkEvent( s1 , s2 ) ; }
} ;

Main::Run * Main::Run::m_this = NULL ;

//static
std::string Main::Run::versionNumber()
{
	return "1.0.0" ;
}

Main::Run::Run( const G::Arg & arg ) :
	m_arg(arg)
{
	if( m_this == NULL )
		m_this = this ;
}

Main::Run::~Run()
{
	if( m_this == this )
		m_this = NULL ;
}

Main::Configuration Main::Run::cfg() const
{
	return cl().cfg() ;
}

std::string Main::Run::smtpIdent() const
{
	return std::string("E-MailRelay V") + versionNumber() ;
}

void Main::Run::closeFiles()
{
	if( cfg().daemon() )
	{
		const bool keep_stderr = true ;
		G::Process::closeFiles( keep_stderr ) ;
	}
}

void Main::Run::closeMoreFiles()
{
	if( cfg().closeStderr() ) // was "daemon && close-stderr", but too confusing
		G::Process::closeStderr() ;
}

bool Main::Run::prepare()
{
	bool do_run = false ;

	if( !runnable() )
	{
	}
	else if( cl().contains("help") )
	{
		cl().showHelp( false ) ;
	}
	else if( cl().hasUsageErrors() )
	{
		cl().showUsageErrors( true ) ;
	}
	else if( cl().contains("version") )
	{
		cl().showVersion( false ) ;
	}
	else if( cl().argc() > 1U )
	{
		cl().showArgcError( true ) ;
	}
	else if( cl().hasSemanticError() )
	{
		cl().showSemanticError( true ) ;
	}
	else
	{
		do_run = true ;
	}

	// early singletons...
	//
	// (prefix,output,log,verbose-log,debug,level,timestamp,strip-context)
	m_log_output <<= new G::LogOutput( m_arg.prefix() , cfg().log() , cfg().log() ,
		cfg().verbose() , cfg().debug() , true ,
		cfg().logTimestamp() , !cfg().debug() ) ;

	return do_run ;
}

void Main::Run::run()
{
	try
	{
		runCore() ;
		G_LOG( "Main::Run::run: done" ) ;
	}
	catch( std::exception & e )
	{
		G_LOG( "Main::Run::run: exception: " << e.what() ) ;
		throw ;
	}
	catch(...)
	{
		G_LOG( "Main::Run::run: unknown exception" ) ;
		throw ;
	}
}

void Main::Run::runCore()
{
	// syslog initialisation
	//
	if( cfg().syslog() )
		G::LogOutput::instance()->syslog(G::LogOutput::Mail) ;

	// fqdn override option
	//
	GNet::Local::fqdn( cfg().fqdn() ) ;

	// daemonising
	//
	G::PidFile pid_file ;
	G::Process::Umask::set( G::Process::Umask::Tightest ) ;
	if( cfg().daemon() ) closeFiles() ; // before opening any sockets or message-store streams
	if( cfg().usePidFile() ) pid_file.init( G::Path(cfg().pidFile()) ) ;
	if( cfg().daemon() ) G::Daemon::detach( pid_file ) ;

	// release root privileges
	//
	G::Root::init( cfg().nobody() ) ;

	// message store singleton
	//
	Main::FileStore store( *this , cfg().spoolDir() ) ;
	if( cfg().useFilter() )
		GSmtp::NewFile::setPreprocessor( G::Path(cfg().filter()) ) ;

	// authentication singleton
	//
	GSmtp::Sasl sasl_library( "emailrelay" , cfg().clientSecretsFile() , cfg().serverSecretsFile() ) ;

	// event loop singletons
	//
	GNet::TimerList timer_list ;
	std::auto_ptr<GNet::EventLoop> event_loop(GNet::EventLoop::create()) ;
	if( ! event_loop->init() )
		throw G::Exception( "cannot initialise network layer" ) ;

	// network monitor singleton
	//
	Main::Monitor monitor( *this ) ;

	// timeout configuration
	//
	GSmtp::Client::responseTimeout( cfg().responseTimeout() ) ;
	GSmtp::Client::connectionTimeout( cfg().connectionTimeout() ) ;

	// run as forwarding agent
	//
	if( cfg().doForwarding() )
	{
		if( store.empty() )
			cl().showNoop( true ) ;
		else
			doForwarding( store , *event_loop.get() ) ;
	}

	// run as storage daemon
	//
	if( cfg().doServing() )
	{
		doServing( pid_file , *event_loop.get() ) ;
	}

	// clean up
	//
	m_client <<= 0 ;
}

void Main::Run::doServing( G::PidFile & pid_file ,
	GNet::EventLoop & event_loop )
{
	std::auto_ptr<GSmtp::Server> smtp_server ;
	if( cfg().doSmtp() )
	{
		GSmtp::Server::AddressList interfaces ;
		if( cfg().interface_().length() )
			interfaces.push_back( GNet::Address(cfg().interface_(),cfg().port()) ) ;

		smtp_server <<= new GSmtp::Server( cfg().port() , interfaces ,
			cfg().allowRemoteClients() , smtpIdent() ,
			cfg().immediate() ? cfg().serverAddress() : std::string() ,
			GSmtp::Verifier(cfg().verifier()) ) ;
	}

	std::auto_ptr<GSmtp::AdminServer> admin_server ;
	if( cfg().doAdmin() )
	{
		admin_server <<= new GSmtp::AdminServer( cfg().adminPort() ,
			cfg().allowRemoteClients() , cfg().serverAddress() ) ;
	}

	{
		G::Root claim_root ;
		pid_file.commit() ;
	}

	closeMoreFiles() ;
	if( smtp_server.get() ) smtp_server->report() ;
	if( admin_server.get() ) admin_server->report() ;
	event_loop.run() ;
}

void Main::Run::doForwarding( GSmtp::MessageStore & store , GNet::EventLoop & event_loop )
{
	const bool quit_on_disconnect = true ;
	GSmtp::Client client( store , *this , quit_on_disconnect ) ;
	std::string error = client.init( cfg().serverAddress() ) ;
	if( error.length() )
		throw G::Exception( error + ": " + cfg().serverAddress() ) ;

	closeMoreFiles() ;
	event_loop.run() ;
}

const Main::CommandLine & Main::Run::cl() const
{
	// lazy evaluation so that the constructor doesnt throw
	if( m_cl.get() == NULL )
	{
		const_cast<Run*>(this)->m_cl <<= new CommandLine( m_arg , versionNumber() ) ;
	}
	return *m_cl.get() ;
}

void Main::Run::clientDone( std::string reason )
{
	G_DEBUG( "Main::Run::clientDone: \"" << reason << "\"" ) ;
	if( ! reason.empty() )
		throw G::Exception( reason ) ;
}

void Main::Run::clientEvent( const std::string & s1 , const std::string & s2 )
{
	onEvent( "client" , s1 , s2 ) ;
}

void Main::Run::raiseStoreEvent()
{
	onEvent( "store" , "update" , std::string() ) ;
}

void Main::Run::raiseNetworkEvent( const std::string & s1 , const std::string & s2 )
{
	onEvent( "network" , s1 , s2 ) ;
}

void Main::Run::onEvent( const std::string & , const std::string & , const std::string & )
{
	; // default implementation does nothing
}

bool Main::Run::runnable() const
{
	return true ; // default implementation
}

//static
bool Main::Run::startForwarding( GSmtp::Client::ClientCallback & callback , const std::string & to )
{
	const char * reason =
		m_this == NULL ?
			"no instance" :
			m_this->startForwarding( &callback , to ) ;

	if( reason != NULL )
	{
		G_DEBUG( "Main::Run::startForwarding: doing nothing: " << reason ) ;
		return false ;
	}
	else
	{
		return true ;
	}
}

const char * Main::Run::startForwarding( GSmtp::Client::ClientCallback * callback , const std::string & to )
{
	if( !GSmtp::MessageStore::exists() ) return "no message store instance" ;
	if( !GNet::EventLoop::exists() ) return "no event loop instance" ;
	if( !GNet::EventLoop::instance().running() ) return "not running the event loop" ;
	if( GSmtp::MessageStore::instance().empty() ) return "nothing to do" ;
	if( m_client.get() != NULL && m_client->busy() ) return "busy" ;

	std::string address( to ) ;
	if( address.empty() )
		address = cfg().serverAddress() ;

	m_client <<= new GSmtp::Client( GSmtp::MessageStore::instance() , *callback , false ) ;
	std::string error = m_client->init( address ) ;
	if( error.length() )
		throw G::Exception( error + ": " + address ) ;
	return NULL ;
}

