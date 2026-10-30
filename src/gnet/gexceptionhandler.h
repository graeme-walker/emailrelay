//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gexceptionhandler.h
///

#ifndef G_NET_EXCEPTION_HANDLER_H
#define G_NET_EXCEPTION_HANDLER_H

#include "gdef.h"
#include "gexceptionsource.h"
#include <exception>

namespace GNet
{
	class ExceptionHandler ;
	class EventHandler ;
}

//| \class GNet::ExceptionHandler
/// An abstract interface for handling exceptions thrown out of
/// event-loop callbacks (socket/future events and timer events).
/// If the handler just rethrows then the event loop will terminate.
///
/// The ExceptionHandler destructor calls disarm() on the EventLoop
/// and TimerList so that an onException() callback is not delivered
/// if the target object has been destroyed.
///
class GNet::ExceptionHandler
{
public:
	virtual ~ExceptionHandler() ;
		///< Destructor. Matching entries in the EventLoop and
		///< TimerList are disarm()ed.

	virtual void onException( ExceptionSource * source , std::exception & e , bool done ) = 0 ;
		///< Called by the event loop when an exception is thrown out
		///< of an event loop callback. The exception is still active
		///< so it can be rethrown with "throw" or captured with
		///< std::current_exception().
		///<
		///< The source parameter can be used to point to the object
		///< that received the original event loop callback. This
		///< requires the appropriate exception source pointer is
		///< defined when the event source is first registered with
		///< the event loop, otherwise it defaults to a null pointer.
		///< (The EventStateUnbound class is used where necessary
		///< to encourage the definition of a valid exception source
		///< pointer.)
		///<
		///< The 'done' parameter indicates whether the exception
		///< was of type GNet::Done.

public:
	ExceptionHandler() = default ;
	ExceptionHandler( const ExceptionHandler & ) = delete ;
	ExceptionHandler( ExceptionHandler && ) = default ;
	ExceptionHandler & operator=( const ExceptionHandler & ) = delete ;
	ExceptionHandler & operator=( ExceptionHandler && ) = default ;
} ;

#endif
