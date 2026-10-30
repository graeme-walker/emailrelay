//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file geventhandler.cpp
///

#include "gdef.h"
#include "geventhandler.h"
#include "gexception.h"
#include "geventloop.h"
#include "glog.h"

GNet::EventHandler::EventHandler()
= default ;

GNet::EventHandler::~EventHandler()
{
	static_assert( noexcept(EventLoop::ptr()) , "" ) ;
	static_assert( noexcept(EventLoop::ptr()->drop(GNet::Descriptor())) , "" ) ;
	EventLoop * event_loop = EventLoop::ptr() ;
	if( event_loop != nullptr )
		event_loop->drop( m_fd ) ;
}

void GNet::EventHandler::readEvent()
{
	G_DEBUG( "GNet::EventHandler::readEvent: no override" ) ;
}

void GNet::EventHandler::writeEvent()
{
	G_DEBUG( "GNet::EventHandler::writeEvent: no override" ) ;
}

void GNet::EventHandler::otherEvent( EventHandler::Reason reason )
{
	throw G::Exception( "socket disconnect event" , str(reason) ) ;
}

std::string GNet::EventHandler::str( EventHandler::Reason reason )
{
	if( reason == EventHandler::Reason::failed ) return "connection failed" ;
	if( reason == EventHandler::Reason::closed ) return "closed" ;
	if( reason == EventHandler::Reason::down ) return "network down" ;
	if( reason == EventHandler::Reason::reset ) return "connection reset by peer" ;
	if( reason == EventHandler::Reason::abort ) return "connection aborted" ;
	return {} ;
}

