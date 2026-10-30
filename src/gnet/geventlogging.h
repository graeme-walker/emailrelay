//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file geventlogging.h
///

#ifndef GNET_EVENT_LOGGING_H
#define GNET_EVENT_LOGGING_H

#include "gdef.h"
#include "gstringview.h"
#include "gassert.h"
#include <string>

namespace GNet
{
	class EventLogging ;
}

//| \class GNet::EventLogging
/// An interface for GNet classes that define a logging context
/// string.
///
/// The EventLogging interface pointer should be installed in an
/// EventState object; then various GNet classes collaborate so
/// that the G::LogOuput context is set appropriately when events
/// are delivered to any objects that inherit copies of that
/// EventState.
///
/// \see GNet::EventState, GNet::EventLoggingContext
///
class GNet::EventLogging
{
public:
	explicit EventLogging( const EventLogging * ) ;
		///< Constructor. Sets the next() pointer.

	virtual ~EventLogging() ;
		///< Destructor.

	virtual std::string_view eventLoggingString() const ;
		///< Returns a string containing logging information
		///< for the object. The string-view should refer to
		///< a string data member or be a nullptr string-view
		///< if there is no logging information.

	const EventLogging * next() const noexcept ;
		///< Returns the link pointer.

public:
	EventLogging( const EventLogging & ) = delete ;
	EventLogging( EventLogging && ) = delete ;
	EventLogging & operator=( const EventLogging & ) = delete ;
	EventLogging & operator=( EventLogging && ) = delete ;

private:
	const EventLogging * m_next {nullptr} ;
} ;

#endif
