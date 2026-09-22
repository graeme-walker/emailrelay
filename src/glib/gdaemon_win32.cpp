//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gdaemon_win32.cpp
///

#include "gdef.h"
#include "gdaemon.h"
#include "gprocess.h"

void G::Daemon::detach()
{
	// no-op
}

void G::Daemon::detach( const G::Path & )
{
	detach() ;
}

