//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gassert.h
///

#ifndef G_ASSERT_H
#define G_ASSERT_H

#include "gdef.h"
#include "glogoutput.h"

#if defined(G_WITH_ASSERT) || ( defined(_DEBUG) && ! defined(G_NO_ASSERT) )
	#define G_ASSERT( test ) G::LogOutput::assertion( G::LogOutput::instance() , __FILE__ , __LINE__ , (test) , #test )
#else
	#define G_ASSERT( test )
#endif

#endif
