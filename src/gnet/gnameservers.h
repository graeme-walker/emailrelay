//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gnameservers.h
///

#ifndef G_NET_NAMESERVERS_H
#define G_NET_NAMESERVERS_H

#include "gdef.h"
#include "gaddress.h"
#include <vector>
#include <string>

namespace GNet
{
	std::vector<Address> nameservers( unsigned int port = 53U ) ;
}

#endif
