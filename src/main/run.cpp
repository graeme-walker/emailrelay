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
// run.cpp
//

#include "gdef.h"
#include "gsmtp.h"
#include "run.h"
#include "gsmtpserver.h"
#include "gsmtpclient.h"
#include "gevent.h"
#include "garg.h"
#include "gdaemon.h"
#include "gfilestore.h"
#include "gnewfile.h"
#include "gadminserver.h"
#include "gmonitor.h"
#include "gexception.h"
#include "gprocess.h"
#include "gmemory.h"
#include "gdebug.h"
#include <iostream>
#include <exception>

//static
std::string Main::Run::versionNumber()
{
	return "0.9.5" ;
}

Main::Run::Run( const G::Arg & arg ) :
	m_arg(arg)
{
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

	if( cl().contains("help") )
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

	return do_run ;
}

void Main::Run::run()
{
	// logging
	//
	G::LogOutput debug( cfg().log() , cfg().verbose() ) ;
	if( cfg().syslog() )
		debug.syslog(G::LogOutput::Mail) ;

	// daemonising
	//
	G::Daemon::PidFile pid_file ;
	G::Process::setUmask() ;
	if( cfg().daemon() )
	{
		closeFiles() ; // before opening any sockets or message-store streams
		if( cfg().usePidFile() )
			pid_file = G::Daemon::PidFile( G::Path(cfg().pidFile()) ) ;
		G::Daemon::detach( pid_file ) ;
	}

	// message store singleton
	//
	GSmtp::FileStore store( cfg().spoolDir() ) ;
	if( cfg().useFilter() )
		GSmtp::NewFile::setPreprocessor( G::Path(cfg().filter()) ) ;

	// event loop singleton
	//
	std::auto_ptr<GNet::EventSources> event_loop(GNet::EventSources::create()) ;
	if( ! event_loop->init() )
		throw G::Exception( "cannot initialise network layer" ) ;

	// network monitor singleton
	//
	GNet::Monitor monitor ;

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
}

void Main::Run::doServing( G::Daemon::PidFile & pid_file ,
	GNet::EventSources & event_loop )
{
	GSmtp::Server server( cfg().port() ,
		cfg().allowRemoteClients() , smtpIdent() ,
		cfg().immediate() ? cfg().serverAddress() : std::string() ) ;

	std::auto_ptr<GSmtp::AdminServer> admin_server ;
	if( cfg().doAdmin() )
	{
		admin_server <<= new GSmtp::AdminServer( cfg().adminPort() ,
			cfg().allowRemoteClients() , cfg().serverAddress() ) ;
	}

	pid_file.commit() ;

	closeMoreFiles() ;
	event_loop.run() ;
}

void Main::Run::doForwarding( GSmtp::MessageStore & store ,
	GNet::EventSources & event_loop )
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
	// lazy evaluation so that the constructor does not throw
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

