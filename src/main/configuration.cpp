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
// configuration.cpp
//

#include "gdef.h"
#include "gsmtp.h"
#include "configuration.h"
#include "commandline.h"
#include "gmessagestore.h"
#include "gstr.h"
#include "gdebug.h"

Main::Configuration::Configuration( const CommandLine & cl ) :
	m_cl(cl)
{
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

bool Main::Configuration::syslog() const
{
	return !m_cl.contains("no-syslog") && !m_cl.contains("as-client") ;
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
