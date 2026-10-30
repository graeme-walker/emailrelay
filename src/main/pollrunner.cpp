//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file pollrunner.cpp
///

#include "gdef.h"
#include "pollrunner.h"
#include "unit.h"
#include "gfile.h"
#include "gstr.h"
#include "gstringtoken.h"
#include "glog.h"
#include "glogoutput.h"
#include "ggettext.h"
#include <sstream>

Main::PollRunner::Config Main::PollRunner::parse( const std::string & spec_in ,
	const G::Path & base_dir , const G::Path & app_dir ,
	G::StringArray * warnings_out )
{
	using G::txt ;
	int verbosity = G::LogOutput::parseVerbosity( spec_in ) ;
	std::string spec = G::LogOutput::noVerbosity( spec_in ) ;
	if( spec.empty() )
	{
		return makeConfig( Config::Type::none , verbosity ) ;
	}
	else if( spec == "unfail:" )
	{
		return makeConfig( Config::Type::unfail , verbosity ) ;
	}
	else if( G::Str::headMatch( spec , "retry:") )
	{
		std::string_view tail = G::Str::tailView( spec , ":" ) ;
		return makeConfig( Config::Type::retry , verbosity , parseNumber(tail) ) ;
	}
	else
	{
		auto config = makeConfig( Config::Type::cmd , verbosity , 0U , fullPath(spec,base_dir,app_dir) ) ;
		if( warnings_out && !G::File::exists(config.path,std::nothrow) )
			warnings_out->emplace_back( std::string(txt("poll-run program does not exist: ")).append(config.path.str()) ) ;
		return config ;
	}
}

Main::PollRunner::PollRunner( GNet::EventState es , Unit & unit , GStore::MessageStore & store ,
	const Config & config , const G::Path & spool_dir , const G::Path & delivery_dir ) :
		m_es(es) ,
		m_unit(unit) ,
		m_store(store) ,
		m_config(config) ,
		m_cmd(config.path,{spool_dir.str(),delivery_dir.str()}),
		m_task(*this,es,GNet::Task::Config()
			.set_exec_error_format("exec error: __strerror__")
			.set_identity(G::Root::nobody())
			.set_force_async())
{
}

Main::PollRunner::~PollRunner()
= default ;

void Main::PollRunner::onTaskDone( int ec , const std::string & output )
{
	logEnd( ec , output ) ;
	if( m_disarmed )
		m_disarmed = false ;
	else
		m_unit.requestForwarding( "poll" ) ;
}

bool Main::PollRunner::busy() const noexcept
{
	static_assert( noexcept(m_task.busy()) , "" ) ;
	return m_task.busy() ;
}

void Main::PollRunner::disarm() noexcept
{
	if( busy() )
		m_disarmed = true ;
}

void Main::PollRunner::start()
{
	G_ASSERT( !busy() ) ; // moot
	if( m_config.type == Config::Type::cmd )
	{
		// unlike filters and address verifiers stdout is not needed for
		// anything functional so capture stdout and stderr for logging
		logStart() ;
		m_task.start( m_cmd , G::Environment::minimal() ,
			G::NewProcess::Fd::devnull() , // stdin
			G::NewProcess::Fd::pipe() , // stdout
			G::NewProcess::Fd::pipe() , // stderr
			G::Path() ) ;
		m_unit.cancelForwarding() ; // for now
	}
	else if( m_config.type == Config::Type::unfail )
	{
		G_LOG_MORE_V( m_config.verbosity , "Main::PollRunner::start: poll runner: unfailing" ) ;
		m_store.unfailAll() ;
	}
	else if( m_config.type == Config::Type::retry )
	{
		G_LOG_MORE_V( m_config.verbosity , "Main::PollRunner::start: poll runner: retrying" ) ;
		m_store.retry( m_config.retry_limit , m_config.verbosity ) ;
	}
}

G::Path Main::PollRunner::fullPath( std::string_view path_in , const G::Path & base_dir , const G::Path & app_dir )
{
	std::string path = G::sv_to_string( path_in ) ;
	if( path.find("@app") == 0U && !app_dir.empty() )
	{
		G::Str::replace( path , "@app" , app_dir.str() ) ;
	}
	else if( G::Path(path).isRelative() && !base_dir.empty() )
	{
		path = (base_dir/path).str() ;
	}
	return {path} ;
}

unsigned int Main::PollRunner::parseNumber( std::string_view s )
{
	for( G::StringTokenView t(s,";",1U) ; t ; ++t )
	{
		if( G::Str::isUInt( t() ) )
			return G::Str::toUInt( t() , 0U ) ;
	}
	return 0U ;
}

void Main::PollRunner::logStart()
{
	G_LOG_MORE_V( m_config.verbosity , "Main::PollRunner::logStart: poll runner [" << m_cmd.exe().basename() << "]: running " << m_cmd.exe() ) ;
}

void Main::PollRunner::logEnd( int ec , const std::string & output )
{
	int verbosity = m_config.verbosity ;
	if( ec == 0 )
		verbosity-- ;
	G_LOG_V( verbosity , "Main::PollRunner::logEnd: poll runner [" << m_task.name() << "]: "
			"exit=" << ec << " output=[" << G::Str::printable(output) << "]" ) ;
}

