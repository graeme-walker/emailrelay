//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gmonitor.h
///

#ifndef G_NET_MONITOR_H
#define G_NET_MONITOR_H

#include "gdef.h"
#include "gslot.h"
#include "gconnection.h"
#include "glistener.h"
#include <memory>
#include <utility>
#include <iostream>

namespace GNet
{
	class Monitor ;
	class MonitorImp ;
}

//| \class GNet::Monitor
/// A singleton for monitoring GNet::Client and GNet::ServerPeer
/// connections.
/// \see GNet::Client, GNet::ServerPeer
///
class GNet::Monitor
{
public:
	Monitor() ;
		///< Default constructor.

	~Monitor() ;
		///< Destructor.

	static Monitor * instance() ;
		///< Returns the singleton pointer. Returns nullptr if none.

	static void addClient( const Connection & client ) ;
		///< Adds a client connection.

	static void removeClient( const Connection & client ) noexcept ;
		///< Removes a client connection.

	static void addServerPeer( const Connection & server_peer ) ;
		///< Adds a server connection.

	static void removeServerPeer( const Connection & server_peer ) noexcept ;
		///< Removes a server connection.

	static void addServer( const Listener & server ) ;
		///< Adds a server.

	static void removeServer( const Listener & server ) noexcept ;
		///< Removes a server.

	void report( std::ostream & stream ,
		const std::string & line_prefix = {} ,
		const std::string & eol = std::string("\n") ) const ;
			///< Reports itself onto a stream.

	void report( G::StringArray & out ) const ;
		///< Reports itself into a three-column table (ordered
		///< with the column index varying fastest).

	G::Slot::Signal<const std::string&,const std::string&> & signal() ;
		///< Provides a callback signal which can be connect()ed
		///< to a slot.
		///<
		///< The signal emits events with two string parameters:
		///< the first is "in", "out" or "listen", and the second
		///< is "start" or "stop".

public:
	Monitor( const Monitor & ) = delete ;
	Monitor( Monitor && ) = delete ;
	Monitor & operator=( const Monitor & ) = delete ;
	Monitor & operator=( Monitor && ) = delete ;

private:
	static Monitor * & pthis() noexcept ;
	std::unique_ptr<MonitorImp> m_imp ;
	G::Slot::Signal<const std::string&,const std::string&> m_signal ;
} ;

#endif
