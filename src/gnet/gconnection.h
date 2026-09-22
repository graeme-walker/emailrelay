//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gconnection.h
///

#ifndef G_NET_CONNECTION_H
#define G_NET_CONNECTION_H

#include "gdef.h"
#include "gaddress.h"

namespace GNet
{
	class Connection ;
}

//| \class GNet::Connection
/// An abstract interface which provides information about a network
/// connection.
/// \see GNet::Client, GNet::ServerPeer
///
class GNet::Connection
{
public:
	virtual ~Connection() = default ;
		///< Destructor.

	virtual Address localAddress() const = 0 ;
		///< Returns the connection's local address.

	virtual Address peerAddress() const = 0 ;
		///< Returns the connection's peer address.
		///< Throws if a client connection that has not yet connected.

	virtual std::string connectionState() const = 0 ;
		///< Returns the connection state as a display string.
		///< This should be the peerAddress() display string, unless
		///< a client connection that has not yet connected.

	virtual std::string peerCertificate() const = 0 ;
		///< Returns the peer's TLS certificate. Returns the
		///< empty string if none.
} ;

#endif
