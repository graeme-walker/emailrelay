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
/// \file pollrunner.h
///

#ifndef MAIN_POLL_RUNNER_H
#define MAIN_POLL_RUNNER_H

#include "gdef.h"
#include "gtask.h"
#include "geventstate.h"
#include "gmessagestore.h"
#include "gexecutablecommand.h"
#include "gstringarray.h"
#include <string>

namespace Main
{
	class PollRunner ;
	class Unit ;
}

//| \class Main::PollRunner
/// Used by Main::Unit to run an external script on every poll cycle.
///
class Main::PollRunner : private GNet::TaskCallback
{
public:
	struct Spec
	{
		enum class Type { none , cmd , unfail , retry } ;
		Type type {Type::none} ;
		unsigned int retry_limit {0U} ;
		bool verbose {false} ;
		G::Path path ;
	} ;

	static Spec parse( const std::string & spec ,
		const G::Path & base_dir , const G::Path & app_dir ,
		G::StringArray * warnings_out = nullptr ) ;
			///< Parses the option value into a Spec structure.

	PollRunner( GNet::EventState , Unit & , GStore::MessageStore & ,
		const Spec & spec , const G::Path & spool_dir , const G::Path & delivery_dir ) ;
			///< Constructor.

	~PollRunner() override ;
		///< Destructor.

	bool busy() const ;
		///< Returns true if busy from last time.

	bool start() ;
		///< Starts the task. Returns true if finished immediately, or calls
		///< Unit::requestForwarding("poll") asynchronously when finished.

public:
	PollRunner( const PollRunner & ) = delete ;
	PollRunner( PollRunner && ) = delete ;
	PollRunner & operator=( const PollRunner & ) = delete ;
	PollRunner & operator=( PollRunner && ) = delete ;

private: // overrides
	void onTaskDone( int , const std::string & ) override ;

private:
	static void fixPath( std::string & , const G::Path & , const G::Path & ) ;
	static unsigned int parseNumber( std::string_view ) ;
	static bool parseVerbose( std::string_view ) ;
	static Spec makeSpec( Spec::Type t , unsigned int n = 0U , bool v = false , const std::string & p = {} ) { Spec s {} ; s.type = t ; s.retry_limit = n ; s.verbose = v ; s.path = G::Path(p) ; return s ; } // c++11

private:
	GNet::EventState m_es ;
	Unit & m_unit ;
	GStore::MessageStore & m_store ;
	Spec m_spec ;
	GNet::Task m_task ;
	G::ExecutableCommand m_cmd ;
} ;

#endif
