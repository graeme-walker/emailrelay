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
#include "gprocess.h"
#include "gmemory.h"
#include "gdebug.h"
#include <iostream>
#include <exception>

//static
std::string Main::Run::versionNumber()
{
	return "0.9.4" ;
}

Main::Run::Run( G::Arg & arg ) :
	m_cl(arg,versionNumber())
{
}

Main::Configuration Main::Run::cfg() const
{
	return m_cl.cfg() ;
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
	if( cfg().daemon() && cfg().closeStderr() )
		G::Process::closeStderr() ;
}

bool Main::Run::prepare()
{
	bool do_run = false ;

	if( m_cl.contains("help") )
	{
		m_cl.showHelp( false ) ;
	}
	else if( m_cl.hasUsageErrors() )
	{
		m_cl.showUsageErrors( true ) ;
	}
	else if( m_cl.contains("version") )
	{
		m_cl.showVersion( false ) ;
	}
	else if( m_cl.argc() > 1U )
	{
		m_cl.showArgcError( true ) ;
	}
	else if( m_cl.hasSemanticError() )
	{
		m_cl.showSemanticError( true ) ;
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
			m_cl.showNoop( true ) ;
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
	GSmtp::Client client( store , quit_on_disconnect ) ;
	std::string error = client.init( cfg().serverAddress() ) ;
	if( error.length() )
		throw G::Exception( error + ": " + cfg().serverAddress() ) ;

	event_loop.run() ;
}

