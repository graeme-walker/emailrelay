//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gfilestore_unix.cpp
///

#include "gdef.h"
#include "gfilestore.h"
#include "gpath.h"
#include "gstrmacros.h"

#ifndef G_SPOOLDIR
	#define G_SPOOLDIR
#endif

G::Path GStore::FileStore::defaultDirectory()
{
	std::string spooldir = G_STR(G_SPOOLDIR) ; // NOLINT readability-redundant-string-init
	if( spooldir.empty() )
		spooldir = "/var/spool/emailrelay" ;
	return { spooldir } ;
}

void GStore::FileStore::osinit()
{
	// no-op
}

