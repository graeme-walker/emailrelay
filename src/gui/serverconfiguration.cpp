//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
// 
// Copyright (c) 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// ===
///
/// \file serverconfiguration.cpp
///

#include "gdef.h"
#include "serverconfiguration.h"
#include "options.h"
#include "gbatchfile.h"
#include "gfile.h"
#include "gstringarray.h"
#include "gstringmap.h"
#include "ggetopt.h"
#include "glog.h"

namespace ServerConfigurationImp
{
	struct Token
	{
		const char * p ;
	} ;
	static std::string & operator<<( std::string & s , Token t )
	{
		s.append(",",s.empty()?0U:1U).append(t.p) ;
		return s ;
	}
}

ServerConfiguration::ServerConfiguration()
= default;

ServerConfiguration::ServerConfiguration( const G::Path & input_file ) :
	m_config(read(input_file))
{
}

G::MapFile ServerConfiguration::read( const G::Path & input_file )
{
	G::MapFile config ;
	if( !G::File::exists(input_file) )
	{
		; // leave it empty
	}
	else if( input_file.extension() == "bat" )
	{
		config = readBatchFile( input_file ) ;
	}
	else
	{
		config = G::MapFile( input_file , "config" ) ;
	}

	normalise( config ) ;
	return config ;
}

G::MapFile ServerConfiguration::readBatchFile( const G::Path & batch_file_path )
{
	const G::BatchFile batch_file( batch_file_path , std::nothrow ) ;
	if( batch_file.args().empty() )
		return {} ;

	// parse the batch file command-line together with the contents of any config file
	G::GetOpt getopt( G::Arg(batch_file.args()) , Main::Options::spec() , Main::Options::config() ) ;
	if( getopt.hasErrors() )
		return {} ;

	return G::MapFile( getopt.map() , G::Str::positive() ) ;
}

void ServerConfiguration::normalise( G::MapFile & config )
{
	// normalise by expanding "--as-whatever" etc.
	std::string_view yes = G::Str::positive() ;
	if( config.contains("as-client") )
	{
		config.update( "log" , yes ) ;
		config.update( "no-syslog" , yes ) ;
		config.update( "no-daemon" , yes ) ;
		config.update( "dont-serve" , yes ) ;
		config.update( "forward" , yes ) ;
		config.update( "forward-to" , config.value("as-client") ) ;
		config.remove( "as-client" ) ;
	}
	if( config.contains("as-proxy") )
	{
		config.update( "log" , yes ) ;
		config.update( "close-stderr" , yes ) ;
		config.update( "forward-on-disconnect" , yes ) ;
		config.update( "forward-to" , config.value("as-proxy") ) ;
		config.remove( "as-proxy" ) ;
	}
	if( config.contains("as-server") )
	{
		config.update( "log" , yes ) ;
		config.update( "close-stderr" , yes ) ;
		config.remove( "as-server" ) ;
	}
	if( config.booleanValue("syslog",false) )
	{
		config.add( "no-syslog" , G::Str::negative() ) ;
	}
	if( config.contains("log-time") )
	{
		config.add( "log-format" , "time" ) ;
		config.remove( "log-time" ) ;
	}
	if( config.contains("log-address") )
	{
		config.add( "log-format" , "address" ) ;
		config.remove( "log-address" ) ;
	}
}

std::string ServerConfiguration::quote( const std::string & s )
{
	return s.find_first_of(" \t") == std::string::npos ? s : (std::string()+"\""+s+"\"") ;
}

std::string ServerConfiguration::exe( const G::Path & batch_file )
{
	return
		G::File::exists(batch_file) &&
		batch_file.extension() == "bat" &&
		!G::BatchFile(batch_file,std::nothrow).args().empty() ?
			G::BatchFile(batch_file,std::nothrow).args().at(0U) :
			std::string() ;
}

ServerConfiguration ServerConfiguration::fromPages( const G::MapFile & pages )
{
	using Token = ServerConfigurationImp::Token ;
	G::StringMap out ;

	std::string auth = G::Path(pages.value("dir-config"),"emailrelay.auth").str() ;

	out["spool-dir"] = pages.value("dir-spool") ;
	out["log"] ;
	out["close-stderr"] ;
	out["pid-file"] = G::Path(pages.value("dir-run"),"emailrelay.pid").str() ;
	if( pages.booleanValue("do-smtp",true) )
	{
		if( pages.booleanValue("forward-immediate",true) )
		{
			out["immediate"] ;
		}
		else if( pages.booleanValue("forward-on-disconnect",true) )
		{
			out["forward-on-disconnect"] ;
		}
		if( pages.booleanValue("forward-poll",true) )
		{
			out["poll"] = pages.value("forward-poll-period") ;
		}
		if( pages.value("smtp-server-port") != "25" )
		{
			out["port"] = pages.value("smtp-server-port") ;
		}
		if( pages.booleanValue("smtp-server-auth",true) )
		{
			out["server-auth"] = auth ;
		}
		if( pages.booleanValue("smtp-server-tls",false) )
		{
			out["server-tls"] ;
			out["server-tls-certificate"] = pages.value("smtp-server-tls-certificate") ;
		}
		else if( pages.booleanValue("smtp-server-tls-connection",false) )
		{
			out["server-tls-connection"] ;
			out["server-tls-certificate"] = pages.value("smtp-server-tls-certificate") ;
		}
		out["forward-to"] = pages.value("smtp-client-host") + ":" + pages.value("smtp-client-port") ;
		if( pages.booleanValue("smtp-client-tls",true) )
		{
			out["client-tls"] ;
		}
		if( pages.booleanValue("smtp-client-tls-connection",true) )
		{
			out["client-tls-connection"] ;
		}
		if( pages.booleanValue("smtp-client-auth",true) )
		{
			out["client-auth"] = auth ;
		}
		if( !pages.value("filter-server").empty() )
		{
			out["filter"] = pages.value("filter-server") ;
		}
		if( !pages.value("filter-client").empty() )
		{
			out["client-filter"] = pages.value("filter-client") ;
		}
		if( !pages.value("address-verifier").empty() )
		{
			out["address-verifier"] = pages.value("address-verifier") ; // script path or "account:"
		}
	}
	else
	{
		out["no-smtp"] ;
	}
	if( pages.booleanValue("do-pop",true) )
	{
		out["pop"] ;
		if( pages.value("pop-port") != "110" )
		{
			out["pop-port"] = pages.value("pop-port") ;
		}
		if( pages.booleanValue("pop-shared-no-delete",true) )
		{
			out["pop-no-delete"] ;
		}
		if( pages.booleanValue("pop-by-name",true) )
		{
			out["pop-by-name"] ;
		}
		out["pop-auth"] = auth ;
	}
	if( pages.booleanValue("logging-verbose",true) )
	{
		out["verbose"] ;
	}
	if( pages.booleanValue("logging-debug",true) )
	{
		out["debug"] ;
	}
	if( !pages.booleanValue("logging-syslog",true) )
	{
		out["no-syslog"] ;
	}
	if( !pages.value("logging-file").empty() )
	{
		out["log-file"] = pages.value("logging-file") ;
	}
	if( pages.booleanValue("logging-time",true) )
	{
		out["log-format"] << Token{"time"} ;
	}
	if( pages.booleanValue("logging-address",true) )
	{
		out["log-format"] << Token{"address"} ;
	}
	if( pages.booleanValue("logging-port",true) )
	{
		out["log-format"] << Token{"port"} ;
	}
	if( pages.booleanValue("logging-msgid",true) )
	{
		out["log-format"] << Token{"msgid"} ;
	}
	if( pages.booleanValue("listening-remote",true) )
	{
		out["remote-clients"] ;
	}
	if( !pages.value("listening-interface").empty() )
	{
		out["interface"] = pages.value("listening-interface") ;
	}

	ServerConfiguration result ;
	result.m_config = G::MapFile( out ) ;
	return result ;
}

const G::MapFile & ServerConfiguration::map() const
{
	return m_config ;
}

