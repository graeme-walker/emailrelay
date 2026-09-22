//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file ghostname.h
///

#ifndef G_HOSTNAME_H
#define G_HOSTNAME_H

#include "gdef.h"
#include <string>

namespace G
{
	std::string hostname() ;
		///< Returns the hostname. This may or may not
		///< relate to the host's name on some network.
		///< Returns the empty string on error.
}

#endif

