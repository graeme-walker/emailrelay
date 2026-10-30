//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gsleep.h
///

#ifndef G_SLEEP_H
#define G_SLEEP_H

#include "gdef.h"
#include <cstdlib>

/// A sleep() function.
///
#ifdef G_WINDOWS
inline void sleep( int s )
{
	::Sleep( s * 1000 ) ;
}
#endif

#endif
