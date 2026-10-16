//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gtask.h
///

#ifndef G_NET_TASK_H
#define G_NET_TASK_H

#include "gdef.h"
#include "geventhandler.h"
#include "genvironment.h"
#include "gnewprocess.h"
#include "geventstate.h"
#include "gexception.h"
#include "gidentity.h"
#include "gexecutablecommand.h"
#include "gtimer.h"
#include <memory>

namespace GNet
{
	class Task ;
	class TaskImp ;
	class TaskCallback ;
}

//| \class GNet::Task
/// A class for running an exectuable in a separate process with an asychronous
/// completion callback.
///
class GNet::Task
{
public:
	G_EXCEPTION( Busy , tx("cannot execute command-line task: still busy from last time") )
	struct Config
	{
		std::string exec_error_format ; // eg. "exec error: __strerror__"
		G::Identity identity {G::Identity::invalid()} ; // eg. G::Root::nobody()
		std::string type ; // for logging
		bool force_async {false} ; // delayed exec errors on Windows
		Config & set_exec_error_format( const std::string & ) ;
		Config & set_identity( const G::Identity & ) ;
		Config & set_type( const std::string & ) ;
		Config & set_force_async( bool = true ) ;
	} ;

	Task( TaskCallback & , EventState es , const Config & ) ;
		///< Constructor for an object that can be start()ed or run().
		///< The type is used for logging. The two trailing parameters
		///< are passed to the G::NewProcess class.

	~Task() ;
		///< Destructor. Kills the spawned process and waits for it to
		///< terminate, where necessary.

	bool busy() const noexcept ;
		///< Returns true if still busy from a prior call to start().

	void start( const G::ExecutableCommand & commandline ) ;
		///< Starts the task by spawning a new process with the given
		///< command-line and also starting a thread to wait for it. The
		///< wait thread signals completion of the child process via the
		///< event loop and the TaskCallback interface. Standard
		///< output goes to the pipe and standard error is discarded.
		///< Throws Busy if still busy from a prior call to start().

	void start( const G::ExecutableCommand & commandline , const G::Environment & env ,
		G::NewProcess::Fd fd_stdin = G::NewProcess::Fd::devnull() ,
		G::NewProcess::Fd fd_stdout = G::NewProcess::Fd::pipe() ,
		G::NewProcess::Fd fd_stderr = G::NewProcess::Fd::devnull() ,
		const G::Path & cd = G::Path() ) ;
			///< Overload with more control over the execution
			///< environment. See also G::NewProcess.

	void stop() ;
		///< Attempts to kill the spawned process. No task-done
		///< callback will be triggered.

	std::pair<int,std::string> run( const G::ExecutableCommand & commandline , const G::Environment & env ,
		G::NewProcess::Fd fd_stdin = G::NewProcess::Fd::devnull() ,
		G::NewProcess::Fd fd_stdout = G::NewProcess::Fd::pipe() ,
		G::NewProcess::Fd fd_stderr = G::NewProcess::Fd::devnull() ,
		const G::Path & cd = G::Path() ) ;
			///< Runs the task synchronously and returns the exit code
			///< and pipe output. Throws if killed. The callback interface
			///< is not used.

public:
	Task( const Task & ) = delete ;
	Task( Task && ) = delete ;
	Task & operator=( const Task & ) = delete ;
	Task & operator=( Task && ) = delete ;

private:
	friend class GNet::TaskImp ;
	void done( int exit_code , const std::string & output ) ;
	void exception( std::exception & ) ;
	void onExecErrorTimeout() ;

private:
	std::unique_ptr<TaskImp> m_imp ;
	TaskCallback & m_callback ;
	EventState m_es ;
	Timer<Task> m_exec_error_timer ;
	std::string m_exec_error ;
	std::string m_name ;
	Config m_config ;
	bool m_busy {false} ;
} ;

//| \class GNet::TaskCallback
/// An abstract interface for callbacks from GNet::Task.
///
class GNet::TaskCallback
{
public:
	virtual ~TaskCallback() = default ;
		///< Destructor.

	virtual void onTaskDone( int exit_status , const std::string & pipe_output ) = 0 ;
		///< Callback function to signal task completion.
} ;

// clang-format off
inline GNet::Task::Config & GNet::Task::Config::set_exec_error_format( const std::string & s ) { exec_error_format = s ; return *this ; }
inline GNet::Task::Config & GNet::Task::Config::set_identity( const G::Identity & i ) { identity = i ; return *this ; }
inline GNet::Task::Config & GNet::Task::Config::set_type( const std::string & s ) { type = s ; return *this ; }
inline GNet::Task::Config & GNet::Task::Config::set_force_async( bool b ) { force_async = b ; return *this ; }
// clang-format on

#endif
