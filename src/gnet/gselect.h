//
// Copyright (C) 2001-2002 Graeme Walker <graeme_walker@users.sourceforge.net>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
// ===
//
// gselect.h
//

#ifndef G_SELECT_H
#define G_SELECT_H

#include "gdef.h"
#include "gnet.h"
#include "gevent.h"
#include "gexception.h"

namespace GNet
{
	class Select ;
} ;

// Class: GNet::Select
// Description: A event-source class which uses ::select().
//
class GNet::Select : public GNet:: EventLoop
{
public:
	G_EXCEPTION( Error , "select() error" ) ;

	Select() ;
		// Constructor.

	virtual ~Select() ;
		// Destructor.

	virtual bool init() ;
		// Override from EventLoop. Does nothing. Returns true.

	virtual void run() ;
		// Override from EventLoop. Runs the event loop.

	virtual void quit() ;
		// Override from EventLoop. Causes run() to return.

	virtual void addRead( Descriptor fd , EventHandler &handler ) ;
		// See EventLoop.

	virtual void addWrite( Descriptor fd , EventHandler &handler ) ;
		// See EventLoop.

	virtual void addException( Descriptor fd , EventHandler &handler ) ;
		// See EventLoop.

	virtual void dropRead( Descriptor fd ) ;
		// See EventLoop.

	virtual void dropWrite( Descriptor fd ) ;
		// See EventLoop.

	virtual void dropException( Descriptor fd ) ;
		// See EventLoop.

private:
	Select( const Select & ) ;
	void operator=( const Select & ) ;
	void runOnce() ;
	virtual void setTimeout( const G::credentials<TimerList> & , G::DateTime::EpochTime t ) ;

private:
	bool m_quit ;
	EventHandlerList m_read_list ;
	EventHandlerList m_write_list ;
	EventHandlerList m_exception_list ;
} ;

#endif
