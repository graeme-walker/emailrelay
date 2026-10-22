//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
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
		int verbosity {-1} ;
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

	void disarm() noexcept ;
		///< Prevents a call to Unit::requestForwarding() when the
		///< current busy() task completes.

	bool busy() const noexcept ;
		///< Returns true if busy.

	void start() ;
		///< Runs or starts the task.
		///< Precondition: !busy()

public:
	PollRunner( const PollRunner & ) = delete ;
	PollRunner( PollRunner && ) = delete ;
	PollRunner & operator=( const PollRunner & ) = delete ;
	PollRunner & operator=( PollRunner && ) = delete ;

private: // overrides
	void onTaskDone( int , const std::string & ) override ;

private:
	void logStart() ;
	void logEnd( int , const std::string & ) ;
	static Spec parsePath( const std::string & , const G::Path & , const G::Path & ) ;
	static G::Path fullPath( std::string_view , const G::Path & , const G::Path & ) ;
	static unsigned int parseNumber( std::string_view ) ;
	static bool parseVerbose( std::string_view ) ;
	static Spec makeSpec( Spec::Type t , unsigned int n = 0U , int verbosity = -1 , const G::Path & p = {} ) { Spec s {} ; s.type = t ; s.retry_limit = n ; s.verbosity = verbosity ; s.path = p ; return s ; } // c++11

private:
	GNet::EventState m_es ;
	Unit & m_unit ;
	GStore::MessageStore & m_store ;
	Spec m_spec ;
	G::ExecutableCommand m_cmd ;
	GNet::Task m_task ;
	bool m_disarmed {false} ;
} ;

#endif
