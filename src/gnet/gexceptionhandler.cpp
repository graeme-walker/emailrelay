//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gexceptionhandler.cpp
///

#include "gdef.h"
#include "gexceptionhandler.h"
#include "gtimerlist.h"
#include "geventloop.h"

GNet::ExceptionHandler::~ExceptionHandler()
{
	if( EventLoop::ptr() )
		EventLoop::ptr()->disarm( this ) ;

	if( TimerList::ptr() )
		TimerList::ptr()->disarm( this ) ;
}

