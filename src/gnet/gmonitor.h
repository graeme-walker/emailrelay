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
// gmonitor.h
//

#ifndef G_GNET_MONITOR_H
#define G_GNET_MONITOR_H

#include "gdef.h"
#include "gnet.h"
#include "gclient.h"
#include "gserver.h"
#include <iostream>

namespace GNet
{
	class Monitor ;
	class MonitorImp ;
} ;

// Class: GNet::Monitor
// Description: A singleton for monitoring Client and ServerPeer connections.
// See also: GNet::Client, GNet::ServerPeer
//
class GNet::Monitor
{
public:
	Monitor() ;
		// Default constructor.

	virtual ~Monitor() ;
		// Destructor.

	static Monitor * instance() ;
		// Returns the singleton pointer. Returns null if none.

	void add( const Client & client ) ;
		// Adds a client.

	void remove( const Client & client ) ;
		// Removes a client.

	void add( const ServerPeer & peer ) ;
		// Adds a server peer.

	void remove( const ServerPeer & peer ) ;
		// Removes a server peer.

	void report( std::ostream & stream ,
		const std::string & line_prefix = std::string() ,
		const std::string & eol = std::string("\n") ) ;
			// Reports itself onto a stream.

private:
	Monitor( const Monitor & ) ; // not implemented
	void operator=( const Monitor & ) ; // not implemented

private:
	static Monitor * m_this ;
	MonitorImp * m_imp ;
} ;

#endif
