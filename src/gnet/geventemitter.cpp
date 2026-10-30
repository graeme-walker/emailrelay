//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file geventemitter.cpp
///

#include "gdef.h"
#include "geventemitter.h"
#include "gnetdone.h"
#include "geventloggingcontext.h"
#include "glog.h"
#include "gassert.h"
#include <functional>

namespace GNet
{
	namespace EventEmitterImp
	{
		template <typename T> void raiseEvent( T handler , EventState & es ) ;

		// for cleaner stack traces compared to std::bind ...
		struct Binder1
		{
			using Method = void (GNet::EventHandler::*)() ;
			Method m_method ;
			EventHandler * m_handler ;
			void operator()() { (m_handler->*m_method)() ; }
		} ;
		struct Binder2
		{
			using Method = void (GNet::EventHandler::*)( EventHandler::Reason ) ;
			Method m_method ;
			EventHandler * m_handler ;
			EventHandler::Reason m_reason ;
			void operator()() { (m_handler->*m_method)( m_reason ) ; }
		} ;
		Binder1 bind( Binder1::Method method , EventHandler * handler ) { return {method,handler} ; }
		Binder2 bind( Binder2::Method method , EventHandler * handler , EventHandler::Reason reason ) { return {method,handler,reason} ; }
	}
}

template <typename T>
void GNet::EventEmitterImp::raiseEvent( T handler , EventState & es )
{
	// see also: std::make_exception_ptr, std::rethrow_exception

	EventLoggingContext set_logging_context( es ) ;
	try
	{
		handler() ; // eg. EventHandler::readEvent()
	}
	catch( GNet::Done & e )
	{
		if( es.hasExceptionHandler() )
			es.doOnException( e , true ) ;
		else
			throw ;
	}
	catch( std::exception & e )
	{
		if( es.hasExceptionHandler() )
			es.doOnException( e , false ) ;
		else
			throw ;
	}
}

// --

void GNet::EventEmitter::raiseReadEvent( EventHandler * handler , EventState & es )
{
	namespace imp = EventEmitterImp ;
	if( handler )
		imp::raiseEvent( imp::bind(&EventHandler::readEvent,handler) , es ) ;
}

void GNet::EventEmitter::raiseWriteEvent( EventHandler * handler , EventState & es )
{
	namespace imp = EventEmitterImp ;
	if( handler )
		imp::raiseEvent( imp::bind(&EventHandler::writeEvent,handler) , es ) ;
}

void GNet::EventEmitter::raiseOtherEvent( EventHandler * handler , EventState & es , EventHandler::Reason reason )
{
	namespace imp = EventEmitterImp ;
	if( handler )
		imp::raiseEvent( imp::bind(&EventHandler::otherEvent,handler,reason) , es ) ;
}

