//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file glocal.h
///

#ifndef G_NET_LOCAL_H
#define G_NET_LOCAL_H

#include "gdef.h"
#include "gaddress.h"
#include "glocation.h"
#include "gexception.h"

namespace GNet
{
	class Local ;
}

//| \class GNet::Local
/// A static class for getting information about the local machine's network
/// name and address.
///
class GNet::Local
{
public:
	static std::string hostname() ;
		///< Returns the local hostname. Returns "localhost" on error.

	static std::string canonicalName() ;
		///< Returns the ASCII fully qualified domain name associated with
		///< hostname(). The result of the first call is 'memoised'.
		///<
		///< On Unix the implementation performs a synchronous DNS query
		///< on the hostname() and returns the canonical name. The hostname
		///< and the returned canonical name are converted to A-labels
		///< if necessary.
		///<
		///< On Windows the 'ComputerNameDnsFullyQualified' value is
		///< returned, also converted to A-labels if necessary.
		///<
		///< Defaults to "<hostname-as-a-label>.localnet" or even
		///< "localhost.localnet" if the result would otherwise be
		///< invalid.

public:
	Local() = delete ;
} ;

#endif
