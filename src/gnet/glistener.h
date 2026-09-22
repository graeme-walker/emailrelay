//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file glistener.h
///

#ifndef G_NET_LISTENER_H
#define G_NET_LISTENER_H

#include "gdef.h"
#include "gaddress.h"

namespace GNet
{
	class Listener ;
}

//| \class GNet::Listener
/// An interface for a network listener.
/// \see GNet::Server, GNet::Monitor
///
class GNet::Listener
{
public:
	virtual ~Listener() = default ;
		///< Destructor.

	virtual Address address() const = 0 ;
		///< Returns the listening address.
} ;

#endif
