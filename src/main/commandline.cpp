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
// commandline.cpp
//

#include "gdef.h"
#include "gsmtp.h"
#include "configuration.h"
#include "commandline.h"
#include "gmessagestore.h"
#include "gstr.h"
#include "gdebug.h"

//static
std::string Main::CommandLine::switchSpec()
{
	std::string dir = GSmtp::MessageStore::defaultDirectory().str() ;
	std::stringstream ss ;
	ss
		<< osSwitchSpec() << "|"
		<< "y!as-proxy!equivalent to \"--log --close-stderr --immediate --forward-to\"!1!host:port|"
		<< "e!close-stderr!closes the standard error stream after start-up!0!|"
		<< "a!admin!enables the administration interface and specifies its listening port number!1!admin-port|"
		<< "x!dont-serve!stops the process acting as a server (usually used with --forward)!0!|"
		<< "z!filter!defines a mail pre-processor (disallowed if running as root)!1!program|"
		<< "f!forward!forwards stored mail on startup (requires --forward-to)!0!|"
		<< "o!forward-to!specifies the remote smtp server (required by --forward and --admin)!1!host:port|"
		<< "h!help!displays help text and exits!0!|"
		<< "m!immediate!forwards each message as soon as it is received (requires --forward-to)!0!|"
		<< "i!pid-file!records the daemon process-id in the given file!1!pid-file|"
		<< "p!port!specifies the smtp listening port number!1!port|"
		<< "r!remote-clients!allows remote clients to connect!0!|"
		<< "s!spool-dir!specifies the spool directory " << "(default is \"" << dir << "\")!1!dir|"
		<< "v!verbose!generates more verbose logging " << "(if compiled-in and logging enabled and stderr open)!0!|"
		<< "V!version!displays version information and exits!0!"
		;
	return ss.str() ;
}

Main::CommandLine::CommandLine( const G::Arg & arg , const std::string & version ) :
	m_version(version) ,
	m_arg(arg) ,
	m_getopt( m_arg , switchSpec() , '|' ,  '!' , '^' )
{
}

unsigned int Main::CommandLine::argc() const
{
	return m_getopt.args().c() ;
}

Main::Configuration Main::CommandLine::cfg() const
{
	return Configuration( *this ) ;
}

bool Main::CommandLine::hasUsageErrors() const
{
	return m_getopt.hasErrors() ;
}

void Main::CommandLine::showUsage( bool e ) const
{
	Show show( e ) ;
	unsigned int columns = ttyColumns() ;
	m_getopt.showUsage( show.s() , m_arg.prefix() , "" , 30U , columns ) ;
}

bool Main::CommandLine::contains( const std::string & name ) const
{
	return m_getopt.contains( name ) ;
}

std::string Main::CommandLine::value( const std::string & name ) const
{
	return m_getopt.value( name ) ;
}

std::string Main::CommandLine::semanticError() const
{
	if( cfg().doAdmin() && cfg().adminPort() == cfg().port() )
	{
		return "the smtp listening port and the "
			"admin listening port must be different" ;
	}

	if( cfg().daemon() && cfg().spoolDir().isRelative() )
	{
		return "in daemon mode the spool-dir must "
			"be an absolute path" ;
	}

	if( !m_getopt.contains("forward-to") && (
		m_getopt.contains("forward") ||
		m_getopt.contains("immediate") ||
		m_getopt.contains("admin") ) )
	{
		return "usage error: the --forward, --immediate and --admin "
			"switches require --forward-to" ;
	}

	return std::string() ;
}

bool Main::CommandLine::hasSemanticError() const
{
	return ! semanticError().empty() ;
}

void Main::CommandLine::showSemanticError( bool e ) const
{
	Show show( e ) ;
	show.s() << m_arg.prefix() << ": " << semanticError() << std::endl ;
}

void Main::CommandLine::showUsageErrors( bool e ) const
{
	Show show( e ) ;
	m_getopt.showErrors( show.s() , m_arg.prefix() ) ;
	showShortHelp( e ) ;
}

void Main::CommandLine::showArgcError( bool e ) const
{
	Show show( e ) ;
	show.s() << m_arg.prefix() << ": usage error: too many non-switch arguments" << std::endl ;
	showShortHelp( e ) ;
}

void Main::CommandLine::showShortHelp( bool e ) const
{
	Show show( e ) ;
	const std::string & exe = m_arg.prefix() ;
	show.s()
		<< std::string(exe.length()+2U,' ')
		<< "try \"" << exe << " --help\" for more information" << std::endl ;
}

void Main::CommandLine::showHelp( bool e ) const
{
	Show show( e ) ;
	showBanner( e ) ;
	show.s() << std::endl ;
	showUsage( e ) ;
	showExtraHelp( e ) ;
	showCopyright( e ) ;
}

void Main::CommandLine::showExtraHelp( bool e ) const
{
	Show show( e ) ;
	const std::string & exe = m_arg.prefix() ;

	show.s() << std::endl ;

	show.s()
		<< "To start a 'storage' daemon in background..." << std::endl
		<< "   " << exe << " --as-server" << std::endl
		<< std::endl ;

	show.s()
		<< "To forward stored mail to \"mail.myisp.co.uk\"..."  << std::endl
		<< "   " << exe << " --as-client mail.myisp.co.uk:smtp" << std::endl
		<< std::endl ;

	show.s()
		<< "To run as a proxy (on port 10025) to a local server (on port 25)..." << std::endl
		<< "   " << exe << " --port 10025 --as-proxy localhost:25" << std::endl
		<< std::endl ;
}

void Main::CommandLine::showNoop( bool e ) const
{
	Show show( e ) ;
	show.s() << m_arg.prefix() << ": no messages to send" << std::endl ;
}

void Main::CommandLine::showBanner( bool e ) const
{
	Show show( e ) ;
	show.s()
		<< "E-MailRelay V" << m_version << std::endl ;
}

void Main::CommandLine::showCopyright( bool e ) const
{
	Show show( e ) ;
	show.s() << copyright() << std::endl ;
}

//static
std::string Main::CommandLine::copyright()
{
	return "Copyright (C) 2001 Graeme Walker" ;
}

void Main::CommandLine::showWarranty( bool e ) const
{
	Show show( e ) ;
	show.s() << warranty() ;
}

//static
std::string Main::CommandLine::warranty( const std::string & eol )
{
	std::stringstream ss ;
	ss
		<< "This software is provided without warranty of any kind." << eol
		<< "You may redistribure copies of this program under " << eol
		<< "the terms of the GNU General Public License." << eol
		<< "For more information refer to the file named COPYING." << eol ;
	return ss.str() ;
}

void Main::CommandLine::showVersion( bool e ) const
{
	Show show( e ) ;
	showBanner( e ) ;
	showWarranty( e ) ;
	showCopyright( e ) ;
}

