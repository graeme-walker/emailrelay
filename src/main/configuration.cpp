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
// configuration.cpp
//

#include "gdef.h"
#include "gsmtp.h"
#include "configuration.h"
#include "commandline.h"
#include "gmessagestore.h"
#include "gstr.h"
#include "gdebug.h"
#include <sstream>

Main::Configuration::Configuration( const CommandLine & cl ) :
	m_cl(cl)
{
}

//static
std::string Main::Configuration::yn( bool b )
{
	return b ? std::string("yes") : std::string("no") ;
}

std::string Main::Configuration::str( const std::string & p , const std::string & eol ) const
{
	const std::string na( "n/a" ) ;
	std::stringstream ss ;
	ss
		<< p << "listening port: " << (doServing()?G::Str::fromUInt(port()):na) << eol
		<< p << "downstream server address: " << (doForwarding()?serverAddress():na) << eol
		<< p << "spool directory: " << spoolDir() << eol
		<< p << "immediate forwarding? " << yn(immediate()) << eol
		<< p << "pre-processor: " << (useFilter()?filter():na) << eol
		<< p << "admin port: " << (doAdmin()?G::Str::fromUInt(adminPort()):na) << eol
		<< p << "run as daemon? " << yn(daemon()) << eol
		<< p << "log to stderr/syslog? " << yn(log()) << eol
		<< p << "verbose logging? " << yn(verbose()) << eol
		<< p << "debug logging? " << yn(debug()) << eol
		//<< p << "use syslog? " << yn(syslog()) << eol
		<< p << "close stderr? " << yn(closeStderr()) << eol
		<< p << "allow remote clients? " << yn(allowRemoteClients()) << eol
		<< p << "pid file: " << (usePidFile()?pidFile():na) << eol
		<< p << "client secrets file: " << clientSecretsFile() << eol
		<< p << "server secrets file: " << serverSecretsFile() << eol
		<< p << "connect timeout: " << connectionTimeout() << "s" << eol
		<< p << "response timeout: " << responseTimeout() << "s" << eol
		<< p << "domain override: " << fqdn() << eol
		;
	return ss.str() ;
}

bool Main::Configuration::log() const
{
	return
		m_cl.contains("log") ||
		m_cl.contains("as-client") ||
		m_cl.contains("as-proxy") ||
		m_cl.contains("as-server") ;
}

bool Main::Configuration::verbose() const
{
	return m_cl.contains("verbose") ;
}

bool Main::Configuration::debug() const
{
	return m_cl.contains("debug") ;
}

bool Main::Configuration::syslog() const
{
	return !m_cl.contains("no-syslog") && !m_cl.contains("as-client") ;
}

bool Main::Configuration::logTimestamp() const
{
	return m_cl.contains("log-time") ;
}

unsigned int Main::Configuration::port() const
{
	return m_cl.contains("port") ?
		G::Str::toUInt(m_cl.value("port")) : 25U ;
}

unsigned int Main::Configuration::adminPort() const
{
	return m_cl.contains("admin") ?
		G::Str::toUInt(m_cl.value("admin")) : 10025U ;
}

bool Main::Configuration::closeStderr() const
{
	return
		m_cl.contains("close-stderr") ||
		m_cl.contains("as-proxy") ||
		m_cl.contains("as-server") ;
}

bool Main::Configuration::immediate() const
{
	return
		m_cl.contains("immediate") ||
		m_cl.contains("as-proxy") ;
}

bool Main::Configuration::daemon() const
{
	return !m_cl.contains("no-daemon") && !m_cl.contains("as-client") ;
}

G::Path Main::Configuration::spoolDir() const
{
	return m_cl.contains("spool-dir") ?
		G::Path(m_cl.value("spool-dir")) :
		GSmtp::MessageStore::defaultDirectory() ;
}

std::string Main::Configuration::serverAddress() const
{
	const char * key = "forward-to" ;
	if( m_cl.contains("as-client") )
		key = "as-client" ;
	else if( m_cl.contains("as-proxy") )
		key = "as-proxy" ;
	return m_cl.contains(key) ? m_cl.value(key) : std::string() ;
}

bool Main::Configuration::doForwarding() const
{
	return m_cl.contains("forward") || m_cl.contains("as-client") ;
}

bool Main::Configuration::doServing() const
{
	return !m_cl.contains("dont-serve") && !m_cl.contains("as-client") ;
}

bool Main::Configuration::allowRemoteClients() const
{
	return m_cl.contains("remote-clients") ;
}

bool Main::Configuration::doAdmin() const
{
	return m_cl.contains("admin") ;
}

bool Main::Configuration::usePidFile() const
{
	return m_cl.contains("pid-file") ;
}

std::string Main::Configuration::pidFile() const
{
	return m_cl.value("pid-file") ;
}

bool Main::Configuration::useFilter() const
{
	return m_cl.contains("filter") ;
}

std::string Main::Configuration::filter() const
{
	return m_cl.value("filter") ;
}

unsigned int Main::Configuration::icon() const
{
	unsigned int n = 0U ;
	if( m_cl.contains("icon") )
	{
		n = G::Str::toUInt(m_cl.value("icon")) ;
		n %= 4U ;
	}
	return n ;
}

std::string Main::Configuration::clientSecretsFile() const
{
	return m_cl.contains("client-auth") ? m_cl.value("client-auth") : std::string() ;
}

std::string Main::Configuration::serverSecretsFile() const
{
	return m_cl.contains("server-auth") ? m_cl.value("server-auth") : std::string() ;
}

unsigned int Main::Configuration::responseTimeout() const
{
	const unsigned int default_timeout = 30U * 60U ;
	return m_cl.contains("response-timeout") ?
		G::Str::toUInt(m_cl.value("response-timeout")) : default_timeout ;
}

unsigned int Main::Configuration::connectionTimeout() const
{
	const unsigned int default_timeout = 40U ;
	return m_cl.contains("connection-timeout") ?
		G::Str::toUInt(m_cl.value("connection-timeout")) : default_timeout ;
}

std::string Main::Configuration::fqdn() const
{
	return m_cl.contains("domain") ? m_cl.value("domain") : std::string() ;
}

std::string Main::Configuration::nobody() const
{
	return m_cl.contains("user") ? m_cl.value("user") : std::string("daemon") ;
}

