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
/// \file pollrunner.cpp
///

#include "gdef.h"
#include "pollrunner.h"
#include "unit.h"
#include "gfile.h"
#include "gstr.h"
#include "gstringtoken.h"
#include "glog.h"
#include "ggettext.h"

Main::PollRunner::Spec Main::PollRunner::parse( const std::string & spec ,
	const G::Path & base_dir , const G::Path & app_dir ,
	G::StringArray * warnings_out )
{
	using G::txt ;
	if( spec.empty() )
	{
		return makeSpec( Spec::Type::none ) ;
	}
	else if( spec == "unfail:" )
	{
		std::string_view tail = G::Str::tailView( spec , ":" ) ;
		return makeSpec( Spec::Type::unfail , 0U , parseVerbose(tail) ) ;
	}
	else if( G::Str::headMatch( spec , "retry:") )
	{
		std::string_view tail = G::Str::tailView( spec , ":" ) ;
		return makeSpec( Spec::Type::retry , parseNumber(tail) , parseVerbose(tail) ) ;
	}
	else
	{
		std::string path( spec ) ;
		fixPath( path , base_dir , app_dir ) ;
		if( warnings_out && !G::File::exists(path,std::nothrow) )
			warnings_out->emplace_back( std::string(txt("poll-run program does not exist: ")).append(path) ) ;
		return makeSpec( Spec::Type::cmd , 0U , false , path ) ;
	}
}

Main::PollRunner::PollRunner( GNet::EventState es , Unit & unit , GStore::MessageStore & store ,
	const Spec & spec , const G::Path & spool_dir , const G::Path & delivery_dir ) :
		m_es(es) ,
		m_unit(unit) ,
		m_store(store) ,
		m_spec(spec) ,
		m_task(*this,es,"poll runner: exec error: __strerror__",G::Root::nobody()) ,
		m_cmd(spec.path,{spool_dir.str(),delivery_dir.str()})
{
}

Main::PollRunner::~PollRunner()
= default ;

void Main::PollRunner::onTaskDone( int ec , const std::string & output )
{
	if( ec != 0 || !G::Str::trimmed(output,G::Str::ws()).empty() )
	{
		G_LOG( "Main::PollRunner::onTaskDone: poll runner: exit " << ec
			<< (output.empty()?"":" (") << G::Str::printable(output) << (output.empty()?"":")") ) ;
	}
	m_unit.requestForwarding( "poll" ) ;
}

bool Main::PollRunner::busy() const
{
	return m_task.busy() ;
}

bool Main::PollRunner::start()
{
	if( m_spec.type == Spec::Type::cmd )
	{
		// unlike filters and address verifiers stdout is not needed for
		// anything functional so discard it and capture stderr instead
		G_LOG_S_IF( m_spec.verbose , "Main::PollRunner::onTaskDone: poll runner: running " << m_cmd.exe() ) ;
		m_task.start( m_cmd , G::Environment::minimal() ,
			G::NewProcess::Fd::devnull() , // stdin
			G::NewProcess::Fd::devnull() , // stdout
			G::NewProcess::Fd::pipe() , // stderr
			G::Path() ) ;
		return false ; // async
	}
	else if( m_spec.type == Spec::Type::unfail )
	{
		G_LOG_S_IF( m_spec.verbose , "Main::PollRunner::onTaskDone: poll runner: unfailing" ) ;
		m_store.unfailAll() ;
		return true ;
	}
	else if( m_spec.type == Spec::Type::retry )
	{
		G_LOG_S_IF( m_spec.verbose , "Main::PollRunner::onTaskDone: poll runner: retrying" ) ;
		m_store.retry( m_spec.retry_limit , m_spec.verbose ) ;
		return true ;
	}
	else
	{
		return true ;
	}
}

void Main::PollRunner::fixPath( std::string & path , const G::Path & base_dir , const G::Path & app_dir )
{
	if( path.find("@app") == 0U && !app_dir.empty() )
	{
		G::Str::replace( path , "@app" , app_dir.str() ) ;
	}
	else if( G::Path(path).isRelative() && !base_dir.empty() )
	{
		path = (base_dir/path).str() ;
	}
}

unsigned int Main::PollRunner::parseNumber( std::string_view s )
{
	for( G::StringTokenView t(s,",",1U) ; t ; ++t )
	{
		if( G::Str::isUInt( t() ) )
			return G::Str::toUInt( t() , 0U ) ;
	}
	return 0U ;
}

bool Main::PollRunner::parseVerbose( std::string_view s )
{
	for( G::StringTokenView t(s,",",1U) ; t ; ++t )
	{
		if( t() == "verbose" )
			return true ;
	}
	return false ;
}

