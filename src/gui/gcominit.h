//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gcominit.h
///

#ifndef G_MAIN_GUI_COM_INIT_H
#define G_MAIN_GUI_COM_INIT_H

#include "gdef.h"
#ifdef G_WINDOWS
#include <objbase.h>
struct GComInit
{
	static void init() { (void) CoInitializeEx(0,0) ; }
	GComInit() { init() ; }
	~GComInit() { CoUninitialize() ; }
	GComInit( const GComInit & ) = delete ;
	GComInit( GComInit && ) = delete ;
	GComInit & operator=( const GComInit & ) = delete ;
	GComInit & operator=( GComInit && ) = delete ;
} ;
#else
struct GComInit
{
} ;
#endif

#endif
