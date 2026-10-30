//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file servicecontrol.h
///

#ifndef G_MAIN_SERVICE_CONTROL_H
#define G_MAIN_SERVICE_CONTROL_H

#include "gdef.h"
#include <string>
#include <utility>

// this interface is used by the GUI installer via Gui::Boot and by the
// service wrapper via ServiceImp (for its "--install" and "--remove"
// options) -- the non-Windows implementations do nothing

std::pair<std::string,DWORD> service_install( const std::string & commandline , const std::string & name ,
	const std::string & display_name , const std::string & description ,
	bool autostart = true ) ;

bool service_installed( const std::string & name ) ;

std::pair<std::string,DWORD> service_remove( const std::string & name ) ;

std::pair<std::string,DWORD> service_start( const std::string & name ) ;

#endif
