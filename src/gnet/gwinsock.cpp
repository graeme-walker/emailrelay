//
// Copyright (C) 2001 Graeme Walker <graeme_walker@users.sourceforge.net>
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
// gwinsock.cpp
//

#include "gdef.h"
#include "gnet.h"
#include "gpump.h"
#include "gappinst.h"
#include "gwinhid.h"
#include "gwinsock.h"
#include "gexception.h"
#include "gtimer.h"
#include "gassert.h"
#include "gdebug.h"
#include "glog.h"

namespace GNet
{
	class WinsockWindow ;
} ;

// Class: GNet::WinsockWindow
// Description: An private implementation class used by GNet::Winsock
// to hook into GGui::Window event processing.
//
class GNet::WinsockWindow : public GGui::WindowHidden
{
public:
	WinsockWindow( Winsock & ws , HINSTANCE h ) ;
private:
	virtual void onWinsock( WPARAM , LPARAM ) ;
	virtual void onTimer( unsigned int ) ;
	Winsock & m_ws ;
} ;

GNet::WinsockWindow::WinsockWindow( Winsock & ws , HINSTANCE hinstance ) :
	m_ws(ws) ,
	GGui::WindowHidden(hinstance)
{
}

void GNet::WinsockWindow::onWinsock( WPARAM w , LPARAM l )
{
	m_ws.onMessage( w , l ) ;
}

void GNet::WinsockWindow::onTimer( unsigned int timer_id )
{
	G_DEBUG( "GNet::WinsockWindow::onTimer: " << timer_id ) ;
	m_ws.onTimer() ;
}

// ===

GNet::Winsock::Winsock() :
	m_window(NULL) ,
	m_read_list("read") ,
	m_write_list("write") ,
	m_exception_list("exception") ,
	m_success(false) ,
	m_hwnd(0) ,
	m_msg(0) ,
	m_timer_id(1U)
{
}

std::string GNet::Winsock::id() const
{
	return m_id ;
}

bool GNet::Winsock::load( const char * )
{
	; // not implemented
	return true ;
}

bool GNet::Winsock::init()
{
	HINSTANCE hinstance = GGui::ApplicationInstance::hinstance() ;
	m_window = new WinsockWindow( *this , hinstance ) ;
	if( m_window->handle() == 0 )
	{
		G_WARNING( "GNet::Winsock::init: cannot create hidden window" ) ;
		return false ;
	}
	return attach( m_window->handle() , GGui::Cracker::wm_winsock() , 1U ) ;
}

bool GNet::Winsock::attach( HWND hwnd , unsigned msg , unsigned int timer_id )
{
	m_hwnd = hwnd ;
	m_msg = msg ;
	m_timer_id = timer_id ;

	WSADATA info ;
	WORD version = MAKEWORD( 1 , 1 ) ;
	int rc = ::WSAStartup( version , &info ) ;
	if( rc != 0 )
	{
		m_reason = "winsock startup failure" ;
		return false ;
	}

	if( LOBYTE( info.wVersion ) != 1 ||
		HIBYTE( info.wVersion ) != 1 )
	{
		m_reason = "incompatible winsock version" ;
		::WSACleanup() ;
		return false ;
	}

	m_id = info.szDescription ;
	G_DEBUG( "GNet::Winsock::attach: winsock \"" << m_id << "\"" ) ;
	m_success = true ;
	return true ;
}

std::string GNet::Winsock::reason() const
{
	return m_reason ;
}

GNet::Winsock::~Winsock()
{
	if( m_success )
		::WSACleanup() ;
	delete m_window ;
}

void GNet::Winsock::addRead( Descriptor fd , EventHandler &handler )
{
	m_read_list.add( fd , &handler ) ;
	update( fd ) ;
}

void GNet::Winsock::addWrite( Descriptor fd , EventHandler &handler )
{
	m_write_list.add( fd , &handler ) ;
	update( fd ) ;
}

void GNet::Winsock::addException( Descriptor fd , EventHandler &handler )
{
	m_exception_list.add( fd , &handler ) ;
	update( fd ) ;
}

void GNet::Winsock::dropRead( Descriptor fd )
{
	m_read_list.remove( fd ) ;
	update( fd ) ;
}

void GNet::Winsock::dropWrite( Descriptor fd )
{
	m_write_list.remove( fd ) ;
	update( fd ) ;
}

void GNet::Winsock::dropException( Descriptor fd )
{
	m_exception_list.remove( fd ) ;
	update( fd ) ;
}

namespace
{
	const long READ_EVENTS = (FD_READ | FD_ACCEPT | FD_OOB) ;
	const long WRITE_EVENTS = (FD_WRITE) ; // no need for "FD_CONNECT"
	const long EXCEPTION_EVENTS = (FD_CLOSE) ;
} ;

void GNet::Winsock::update( Descriptor fd )
{
	G_ASSERT( m_success ) ;
	G_ASSERT( m_hwnd != 0 ) ;
	G_ASSERT( m_msg != 0 ) ;
	::WSAAsyncSelect( fd , m_hwnd , m_msg , desiredEvents(fd) ) ;
}

long GNet::Winsock::desiredEvents( Descriptor fd )
{
	long mask = 0 ;

	if( m_read_list.contains(fd) )
		mask |= READ_EVENTS ;

	if( m_write_list.contains(fd) )
		mask |= WRITE_EVENTS ;

	if( m_exception_list.contains(fd) )
		mask |= EXCEPTION_EVENTS ;

	return mask ;
}

GNet::EventHandler * GNet::Winsock::findHandler( EventHandlerList &list , Descriptor fd )
{
	return list.find( fd ) ;
}

void GNet::Winsock::onMessage( WPARAM wparam , LPARAM lparam )
{
	int fd = wparam ;
	int event = WSAGETSELECTEVENT( lparam ) ;
	int err = WSAGETSELECTERROR( lparam ) ;

	G_DEBUG( "GNet::Winsock::processMessage: winsock select message: "
		<< "w=" << wparam << " l=" << lparam
		<< " fd=" << fd << " evt=" << event << " err=" << err ) ;

	if( event & READ_EVENTS )
	{
		EventHandler *handler = findHandler( m_read_list , fd ) ;
		if( handler )
			handler->readEvent();
	}
	else if( event & WRITE_EVENTS )
	{
		EventHandler *handler = findHandler( m_write_list , fd ) ;
		if( handler )
			handler->writeEvent();
	}
	else if( event & EXCEPTION_EVENTS )
	{
		EventHandler *handler = findHandler( m_exception_list , fd ) ;
		if( handler )
			handler->exceptionEvent();
	}
	else if( err )
	{
		G_DEBUG( "GNet::Winsock::processMessage: winsock select error: " << err ) ;
	}
	else
	{
		G_DEBUG( "GNet::Winsock::processMessage: unwanted winsock event: " << event ) ;
	}
}

void GNet::Winsock::onTimer()
{
	G_DEBUG( "GNet::Winsock::onTimer" ) ;
	::KillTimer( m_hwnd , m_timer_id ) ; // since periodic
	TimerList::instance().doTimeouts() ;
}

void GNet::Winsock::setTimeout( G::DateTime::EpochTime t )
{
	G_DEBUG( "GNet::Winsock::setTimeout: " << t ) ;
	if( t != 0U )
	{
		G::DateTime::EpochTime now = G::DateTime::now() ;
		unsigned int interval = t > now ? (t - now) : 0U ;
		unsigned long ms = interval ;
		ms *= 1000UL ;
		G_DEBUG( "GNet::Winsock::setTimeout: SetTimer(): " << ms << "ms" ) ;
		::KillTimer( m_hwnd , m_timer_id ) ;
		unsigned int rc = ::SetTimer( m_hwnd , m_timer_id , ms , NULL ) ;
		if( rc == 0U )
			throw G::Exception( "GNet::Winsock: SetTimer() failure" ) ;
		G_ASSERT( rc == m_timer_id ) ;
	}
	else
	{
		G_DEBUG( "GNet::Winsock::setTimeout: KillTimer()" ) ;
		::KillTimer( m_hwnd , m_timer_id ) ;
	}
}

void GNet::Winsock::run()
{
	GGui::Pump::run() ;
}

void GNet::Winsock::quit()
{
	GGui::Pump::quit() ;
}

