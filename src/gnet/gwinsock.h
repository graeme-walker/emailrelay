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
// gwinsock.h
//

#ifndef G_WINSOCK_H
#define G_WINSOCK_H

#include "gdef.h"
#include "gnet.h"
#include "gwinbase.h"
#include "gevent.h"

namespace GNet
{
	class Winsock ;
} ;

// Class: GNet::Winsock
// Description: A windows-specific class which initialises
// the WinSock system, and integrates the windows message
// queue with it. Note that WinSock events are delivered
// as messages to the application's main message queue,
// and these messages have to be passed on the the
// WinSock layer.
//
class GNet::Winsock : public EventSources
{
public:
	Winsock() ;
		// Default constructor. Initialise with load() (optional)
		// and then either init() or attach().

	bool load( const char * dllpath ) ;
		// Loads the given WinSock DLL. Must be the first
		// method called after construction.
		// Returns false on error.

	virtual bool init() ;
		// Override from EventSources. Initialses the
		// WinSock library, passing it the handle
		// of an internally-created hidden window.
		// Returns false on error.

	bool attach( HWND hwnd , unsigned int msg ) ;
		// Initialises the WinSock library, passin
		// it the specified window handle an
		// message number. WinSock events are sent
		// to that window. Returns false on error.
		//
		// For simple, synchronous programs
		// the window handle may be zero.

	std::string reason() const ;
		// Returns the reason for initialisation
		// failure.

	std::string id() const ;
		// Returns the WinSock implementation's
		// identification string. Returns the
		// zero length string on error.

	virtual ~Winsock() ;
		// Destructor. Releases the WinSock resources
		// if this is the last Winsock object.

	void onMessage( WPARAM wparam , LPARAM lparam ) ;
		// To be called on receipt of a window
		// message corresponding to the constructor's
		// 'msg' parameter.

	virtual void run() ;
		// Override from EventSources. Calls GGui::Pump::run()

	virtual void quit() ;
		// Override from EventSources. Calls GGui::Pump::quit().

protected:
	virtual void addRead( Descriptor fd , EventHandler & handler ) ;
	virtual void addWrite( Descriptor fd , EventHandler & handler ) ;
	virtual void addException( Descriptor fd , EventHandler & handler ) ;
	virtual void dropRead( Descriptor fd ) ;
	virtual void dropWrite( Descriptor fd ) ;
	virtual void dropException( Descriptor fd ) ;

private:
	Winsock( const Winsock & other ) ;
	void operator=( const Winsock & other ) ;
	EventHandler *findHandler( EventHandlerList & list , Descriptor fd ) ;
	void update( Descriptor fd ) ;
	long desiredEvents( Descriptor fd ) ;

private:
	bool m_initialised ;
	GGui::WindowBase * m_window ;
	bool m_success ;
	std::string m_reason ;
	std::string m_id ;
	HWND m_hwnd ;
	unsigned m_msg ;
	EventHandlerList m_read_list ;
	EventHandlerList m_write_list ;
	EventHandlerList m_exception_list ;
} ;

#endif


