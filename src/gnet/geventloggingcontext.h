//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file geventloggingcontext.h
///

#ifndef G_NET_EVENT_LOGGING_CONTEXT_H
#define G_NET_EVENT_LOGGING_CONTEXT_H

#include "gdef.h"
#include "geventlogging.h"
#include "geventstate.h"
#include "gstringview.h"

namespace GNet
{
	class EventLoggingContext ;
}

//| \class GNet::EventLoggingContext
/// A class that sets the G::LogOuput::context() while in scope.
///
class GNet::EventLoggingContext
{
public:
	explicit EventLoggingContext( EventState ) ;
		///< Constructor that sets the G::LogOutput logging context to
		///< the accumulation of EventLogging::eventLoggingString()s.

	explicit EventLoggingContext( std::string_view ) ;
		///< Constructor that sets the G::LogOutput logging context to
		///< the given string.

	EventLoggingContext( EventState , const std::string & ) ;
		///< Constructor that sets the G::LogOutput logging context to
		///< the accumulation of EventLogging::eventLoggingString()s
		///< and the given string.

	~EventLoggingContext() ;
		///< Destructor. Restores the logging context.

public:
	EventLoggingContext( const EventLoggingContext & ) = delete ;
	EventLoggingContext( EventLoggingContext && ) = delete ;
	EventLoggingContext & operator=( const EventLoggingContext & ) = delete ;
	EventLoggingContext & operator=( EventLoggingContext && ) = delete ;

private:
	static std::string_view fn( void * ) ;
	static void set( std::string & , EventState ) ;

private:
	static EventLoggingContext * m_inner ;
	EventLoggingContext * m_outer ;
	static std::string m_s ;
} ;

#endif
