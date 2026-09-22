//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file geventemitter.h
///

#ifndef G_NET_EVENT_EMITTER_H
#define G_NET_EVENT_EMITTER_H

#include "gdef.h"
#include "geventhandler.h"
#include "geventstate.h"

namespace GNet
{
	class EventEmitter ;
}

//| \class GNet::EventEmitter
/// Provides static methods to raise an EventHandler event, as used
/// by the various event loop implementations.
///
/// Any exceptions thrown by an event handler are caught and delivered
/// to the associated exception handler.
///
/// Event loop implementations are required to keep the EventState
/// object valid when using this interface, even if the event handler
/// deletes the target object(s) (see EventLoop::disarm()).
///
class GNet::EventEmitter
{
public:
	static void raiseReadEvent( EventHandler * , EventState & ) ;
		///< Calls readEvent() on the event handler and catches any
		///< exceptions and delivers them to the EventState exception
		///< handler.

	static void raiseWriteEvent( EventHandler * , EventState & ) ;
		///< Calls writeEvent() on the event handler and catches any
		///< exceptions and delivers them to the EventState exception
		///< handler.

	static void raiseOtherEvent( EventHandler * , EventState & , EventHandler::Reason ) ;
		///< Calls otherEvent() on the event handler and catches any
		///< exceptions and delivers them to the EventState exception
		///< handler.

public:
	EventEmitter() = delete ;
} ;

#endif
