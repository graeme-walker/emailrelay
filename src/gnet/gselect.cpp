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
// gselect.cpp
//

#include "gdef.h"
#include "gselect.h"
#include "gstr.h"
#include "gtimer.h"
#include "gdebug.h"
#include <sys/types.h>
#include <sys/time.h>

typedef struct timeval Timeval ; // std:: ??

namespace GNet
{
	class Lock ;
	class FdSet ;
} ;

// Class: GNet::Lock
// Description: A private implementation class used by GNet::Select to
// lock data structures in the face of reentrancy.
//
class GNet::Lock
{
public:
	EventHandlerList & m_list ;
	explicit Lock( EventHandlerList & list ) ;
	~Lock() ;
private:
	Lock( const Lock & ) ;
	void operator=( const Lock & ) ;
} ;

// Class: GNet::FdSet
// Description: A static implementation interface used by GNet::Select
// to do fd_set iteration.
//
class GNet::FdSet
{
public:
	static int init( int n , fd_set * set , const EventHandlerList & list ) ;
	static void raiseEvents( fd_set * set , EventHandlerList & list ,
		void (EventHandler::*method)() , const char * type ) ;
private:
	FdSet() ;
} ;

// ===

inline
GNet::Lock::Lock( EventHandlerList & list ) :
	m_list(list)
{
	m_list.lock() ;
}

inline
GNet::Lock::~Lock()
{
	m_list.unlock() ;
}

// ===

//static
int GNet::FdSet::init( int n , fd_set * set , const EventHandlerList & list )
{
	FD_ZERO( set ) ;
	const EventHandlerList::Iterator end = list.end() ;
	for( EventHandlerList::Iterator p = list.begin() ; p != end ; ++p )
	{
		Descriptor fd = EventHandlerList::fd( p ) ;
		FD_SET( fd , set ) ;
		if( (fd+1) > n )
			n = (fd+1) ;
	}
	return n ;
}

//static
void GNet::FdSet::raiseEvents( fd_set * set , EventHandlerList & list ,
	void (EventHandler::*method)() , const char * /*type*/ )
{
	GNet::Lock lock( list ) ; // since event handlers may change the list while we iterate
	const EventHandlerList::Iterator end = list.end() ;
	for( EventHandlerList::Iterator p = list.begin() ; p != end ; ++p )
	{
		Descriptor fd = EventHandlerList::fd( p ) ;
		if( FD_ISSET( fd , set ) )
		{
			//G_DEBUG( "raiseEvents: " << type << " event on fd " << fd ) ;
			EventHandler & h = EventHandlerList::handler( p ) ;
			(h.*method)() ;
		}
	}
}

// ===

GNet::Select::Select() :
	m_quit(false) ,
	m_read_list(std::string("read")) ,
	m_write_list(std::string("write")) ,
	m_exception_list(std::string("exception"))
{
}

GNet::Select::~Select()
{
}

bool GNet::Select::init()
{
	return true ;
}

void GNet::Select::run()
{
	while( !m_quit )
	{
		runOnce() ;
	}
	m_quit = false ;
}

void GNet::Select::quit()
{
	m_quit = true ;
}

void GNet::Select::runOnce()
{
	int n = 1 ;
	fd_set r ; n = FdSet::init( n , &r , m_read_list ) ;
	fd_set w ; n = FdSet::init( n , &w , m_write_list ) ;
	fd_set e ; n = FdSet::init( n , &e , m_exception_list ) ;

	Timeval timeout ;
	Timeval * timeout_p = NULL ;
	if( TimerList::instance(TimerList::NoThrow()) != NULL )
	{
		bool infinite = false ;
		timeout.tv_sec = TimerList::instance().interval( infinite ) ;
		timeout.tv_usec = 0 ; // micro seconds
		timeout_p = infinite ? NULL : &timeout ;
	}

	const bool debug = false ;
	if( debug )
	{
		G_DEBUG( "GNet::Select::runOnce: selecting: fd(max) = " << (n-1) << ": "
			<< "read-list=\"" << m_read_list.asString() << "\": "
			<< "write-list=\"" << m_write_list.asString() << "\": "
			<< "exception-list=\"" << m_exception_list.asString() << "\": "
			<< "timeout=" << (timeout_p?G::Str::fromUInt(timeout_p->tv_sec):std::string("infinite")) ) ;
	}

	int rc = ::select( n , &r , &w , &e , timeout_p ) ;
	if( rc == 0 )
	{
		G_DEBUG( "GNet::Select::runOnce: select() timeout" ) ;
		TimerList::instance().doTimeouts() ;
	}
	else if( rc > 0 )
	{
		G_DEBUG( "GNet::Select::runOnce: detected event(s) on " << rc << " fd(s)" ) ;
		FdSet::raiseEvents( &r , m_read_list , & EventHandler::readEvent , "read" ) ;
		FdSet::raiseEvents( &w , m_write_list , & EventHandler::writeEvent , "write" ) ;
		FdSet::raiseEvents( &e , m_exception_list , & EventHandler::exceptionEvent , "exception" ) ;
	}
	else
	{
		throw Error() ;
	}
}

void GNet::Select::addRead( Descriptor fd , EventHandler & handler )
{
	m_read_list.add( fd , & handler ) ;
}

void GNet::Select::addWrite( Descriptor fd , EventHandler & handler )
{
	m_write_list.add( fd , & handler ) ;
}

void GNet::Select::addException( Descriptor fd , EventHandler & handler )
{
	m_exception_list.add( fd , & handler ) ;
}

void GNet::Select::dropRead( Descriptor fd )
{
	m_read_list.remove( fd ) ;
}

void GNet::Select::dropWrite( Descriptor fd )
{
	m_write_list.remove( fd ) ;
}

void GNet::Select::dropException( Descriptor fd )
{
	m_exception_list.remove( fd ) ;
}

void GNet::Select::setTimeout( const G::credentials<TimerList> & , G::DateTime::EpochTime )
{
	// not used -- interval() in runOnce() suffices
}

