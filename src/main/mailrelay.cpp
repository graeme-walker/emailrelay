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
// mailrelay.cpp
//

#include "gdef.h"
#include "gsmtp.h"
#include "gsmtpserver.h"
#include "gsmtpclient.h"
#include "gevent.h"
#include "garg.h"
#include "gdaemon.h"
#include "gstr.h"
#include "gpath.h"
#include "gmessagestore.h"
#include "gadminserver.h"
#include "gexception.h"
#include "ggetopt.h"
#include "gdebug.h"
#include <iostream>
#include <exception>

namespace
{
	class Main
	{
	public:
		explicit Main( G::Arg & arg ) ;
		void run() ;
	private:
		std::string versionNumber() const ;
		void version() const ;
		void banner() const ;
		void warranty() const ;
		void usage( std::ostream & s , const std::string & exe , const G::GetOpt & opt ) const ;
		void help( const std::string & exe ) const ;
		void shortHelp( std::ostream & stream , const std::string & exe ) const ;
		void copyright() const ;
		std::string switchSpec() const ;
		void runCore() ;
		unsigned int ttyColumns() const ;
		std::string checkOptions() const ;
		const G::GetOpt & opt() const ;
		unsigned int optPort() const ;
		unsigned int optAdminPort() const ;
		bool optCloseStderr() const ;
		bool optLog() const ;
		bool optSyslog() const ;
		bool optDaemon() const ;
		bool optDoForwarding() const ;
		bool optDoServing() const ;
		bool optDoAdmin() const ;
		bool optAllowRemoteClients() const ;
		G::Path optSpoolDir() const ;
		std::string optServerAddress() const ;
		void doForwarding( GSmtp::MessageStore & store ) ;
		void closeFiles() ;
		void closeMoreFiles() ;
		std::string smtpIdent() const ;
		void recordPid() ;
	private:
		G::Arg m_arg ;
		G::GetOpt * m_opt ;
	} ;
} ;

Main::Main( G::Arg & arg ) :
	m_arg(arg)
{
}

void Main::banner() const
{
	std::cout
		<< "E-MailRelay V" << versionNumber() << std::endl ;
}

void Main::copyright() const
{
	std::cout << "Copyright (C) 2001 Graeme Walker" << std::endl ;
}

void Main::warranty() const
{
	std::cout
		<< "This software is provided without warranty of any kind." << std::endl
		<< "You may redistribure copies of this program under the terms of the GNU "
		<< "General Public License." << std::endl
		<< "For more information refer to the file named COPYING." << std::endl ;
}

void Main::version() const
{
	banner() ;
	warranty() ;
	copyright() ;
}

std::string Main::versionNumber() const
{
	return "0.9.1" ;
}

std::string Main::smtpIdent() const
{
	return std::string("E-MailRelay V") + versionNumber() ;
}

unsigned int Main::ttyColumns() const
{
	const unsigned int default_ = 79U ;
	try
	{
		const char * p = std::getenv( "COLUMNS" ) ;
		return p ? G::Str::toUInt(p) : default_ ;
	}
	catch( std::exception & )
	{
		return default_ ;
	}
}

void Main::usage( std::ostream & s , const std::string & exe , const G::GetOpt & opt ) const
{
	opt.showUsage( s , exe , "" , 30U , ttyColumns() ) ;
}

void Main::help( const std::string & exe ) const
{
	std::cout << std::endl ;

	std::cout
		<< "To start a 'storage' daemon in background..." << std::endl
		<< "   " << exe << " --as-server" << std::endl
		<< std::endl ;

	std::cout
		<< "To forward stored mail to \"mail.myisp.co.uk\"..."  << std::endl
		<< "   " << exe << " --as-client mail.myisp.co.uk:smtp" << std::endl
		<< std::endl ;

	std::cout
		<< "To start a 'store & forward' daemon..." << std::endl
		<< "   " << exe << " --as-server --admin 10025 --forward-to mail.myisp.co.uk:smtp" << std::endl
		<< "     (and then \"" << exe << "poke 10025\" to trigger forwarding)" << std::endl
		<< std::endl ;
}

void Main::shortHelp( std::ostream & stream , const std::string & exe ) const
{
	stream
		<< std::string(exe.length()+2U,' ')
		<< "try \"" << exe << " --help\" for more information" << std::endl ;
}

std::string Main::switchSpec() const
{
	std::stringstream ss ;
	ss
		<< "h!help!displays help text and exits!0!|"
		<< "l!log!writes log information on standard error (if open) and syslog (if not disabled)!0!|"
		<< "v!verbose!generates more verbose logging "
			<< "(if compiled-in and logging enabled and stderr open)!0!|"
		<< "e!close-stderr!closes the standard error stream when daemonising!0!|"
		<< "s!spool-dir!specifies the spool directory "
			<< "(default is \"" << GSmtp::MessageStore::defaultDirectory()
			<< "\")!1!dir|"
		<< "q!as-client!equivalent to \"--no-syslog --no-daemon --log --dont-serve --forward --forward-to\"!"
			<< "1!host:port|"
		<< "d!as-server!equivalent to \"--close-stderr --log\"!0!|"
		<< "n!no-syslog!disables syslog output!0!|"
		<< "t!no-daemon!does not detach from the terminal!0!|"
		<< "x!dont-serve!stops the process acting as a server (usually used with --forward)!0!|"
		<< "f!forward!forwards stored mail on startup (requires --forward-to)!0!|"
		<< "o!forward-to!specifies the remote smtp server (required by --forward and --admin)!1!host:port|"
		<< "r!remote-clients!allows remote clients to connect!0!|"
		<< "i!pid-file!records the daemon process-id in the given file!1!pid-file|"
		<< "p!port!specifies the smtp listening port number!1!port|"
		<< "a!admin!enables the administration interface and specifies its listening port number!1!admin-port|"
		<< "V!version!displays version information and exits!0!"
		;
	return ss.str() ;
}

const G::GetOpt & Main::opt() const
{
	G_ASSERT( m_opt != NULL ) ;
	return *m_opt ;
}

void Main::run()
{
	m_opt = new G::GetOpt( m_arg , switchSpec() , '|' , '!' , '^' ) ;

	if( opt().contains("help") )
	{
		banner() ;
		std::cout << std::endl ;
		usage( std::cout , m_arg.prefix() , opt() ) ;
		help( m_arg.prefix() ) ;
		copyright() ;
	}
	else if( opt().hasErrors() )
	{
		opt().showErrors( std::cerr , m_arg.prefix() ) ;
		shortHelp( std::cerr , m_arg.prefix() ) ;
	}
	else if( opt().args().c() > 1U )
	{
		std::cerr << m_arg.prefix() << ": usage error: too many non-switch arguments" << std::endl ;
		shortHelp( std::cerr , m_arg.prefix() ) ;
	}
	else if( opt().contains("version") )
	{
		version() ;
	}
	else
	{
		G::LogOutput debug( optLog() , opt().contains("verbose") ) ;
		if( optSyslog() )
			debug.syslog(G::LogOutput::Mail) ;

		runCore() ;
	}
}

bool Main::optLog() const
{
	return opt().contains("log") || opt().contains("as-client") || opt().contains("as-server") ;
}

bool Main::optSyslog() const
{
	return !opt().contains("no-syslog") && !opt().contains("as-client") ;
}

unsigned int Main::optPort() const
{
	return opt().contains("port") ?
		G::Str::toUInt(opt().value("port")) : 25U ;
}

unsigned int Main::optAdminPort() const
{
	return opt().contains("admin") ?
		G::Str::toUInt(opt().value("admin")) : 10025U ;
}

bool Main::optCloseStderr() const
{
	return opt().contains("close-stderr") || opt().contains("as-server") ;
}

bool Main::optDaemon() const
{
	return !opt().contains("no-daemon") && !opt().contains("as-client") ;
}

G::Path Main::optSpoolDir() const
{
	return opt().contains("spool-dir") ?
		G::Path(opt().value("spool-dir")) :
		GSmtp::MessageStore::defaultDirectory() ;
}

std::string Main::optServerAddress() const
{
	const char * key = opt().contains("forward-to") ? "forward-to" : "as-client" ;
	return opt().contains(key) ?  opt().value(key) : std::string() ;
}

std::string Main::checkOptions() const
{
	if( optDoAdmin() && optAdminPort() == optPort() )
	{
		return "the smtp listening port and the "
			"admin listening port must be different" ;
	}

	if( optDaemon() && optSpoolDir().isRelative() )
	{
		return "in daemon mode the spool-dir must "
			"be an absolute path (starting with /)" ;
	}

	if( !opt().contains("forward-to") &&
		(opt().contains("admin") || opt().contains("forward")) )
	{
		return "usage error: the --admin and --forward "
			"switches require --forward-to" ;
	}

	return std::string() ;
}

bool Main::optDoForwarding() const
{
	return opt().contains("forward") || opt().contains("as-client") ;
}

void Main::doForwarding( GSmtp::MessageStore & store )
{
	const bool quit_on_disconnect = true ;
	GSmtp::Client client( store , quit_on_disconnect ) ;
	std::string error = client.init( optServerAddress() ) ;
	if( error.length() )
		throw G::Exception( error + ": " + optServerAddress() ) ;

	GNet::EventSources::instance().run() ;
}

void Main::closeFiles()
{
	if( optDaemon() )
	{
		const bool keep_stderr = true ;
		G::Daemon::closeFiles( keep_stderr ) ;
	}
}

void Main::closeMoreFiles()
{
	if( optDaemon() && optCloseStderr() )
		G::Daemon::closeStderr() ;
}

bool Main::optDoServing() const
{
	return !opt().contains("dont-serve") && !opt().contains("as-client") ;
}

bool Main::optAllowRemoteClients() const
{
	return opt().contains("remote-clients") ;
}

bool Main::optDoAdmin() const
{
	return opt().contains("admin") ;
}

void Main::runCore()
{
	std::string error = checkOptions() ;
	if( !error.empty() )
		throw G::Exception( error ) ;

	G::Daemon::setUmask() ;
	if( optDaemon() )
	{
		closeFiles() ; // before opening any sockets or message-store streams
		if( opt().contains("pid-file") )
			G::Daemon::detach( G::Path(opt().value("pid-file")) ) ;
		else
			G::Daemon::detach() ;
	}

	GSmtp::MessageStore store( optSpoolDir() ) ;

	std::auto_ptr<GNet::EventSources> event_loop( GNet::EventSources::create() ) ;
	if( ! event_loop->init() )
		throw G::Exception( "cannot initialise network layer" ) ;

	if( optDoForwarding() )
	{
		if( store.empty() )
			std::cerr << m_arg.prefix() << ": no messages to send" << std::endl ;
		else
			doForwarding( store ) ;
	}

	if( optDoServing() )
	{
		GSmtp::Server server( optPort() , optAllowRemoteClients() , smtpIdent() ) ;

		if( optDoAdmin() )
		{
			GSmtp::AdminServer admin_server( optAdminPort() ,
				optAllowRemoteClients() , optServerAddress() ) ;
			closeMoreFiles() ;
			event_loop->run() ;
		}
		else
		{
			closeMoreFiles() ;
			event_loop->run() ;
		}
	}
}

int main( int argc , char * argv [] )
{
	G::Arg arg( argc , argv ) ;
	try
	{
		Main main( arg ) ;
		main.run() ;
		return EXIT_SUCCESS ;
	}
	catch( std::exception & e )
	{
		std::cerr << arg.prefix() << ": exception: " << e.what() << std::endl ;
	}
	catch( ... )
	{
		std::cerr << arg.prefix() << ": unrecognised exception" << std::endl ;
	}
	return EXIT_FAILURE ;
}


