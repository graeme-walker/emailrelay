//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file ghostname_win32.cpp
///

#include "gdef.h"
#include "gnowide.h"
#include "ghostname.h"
#include "genvironment.h"
#include "gstr.h"

std::string G::hostname()
{
	std::string name = nowide::getComputerNameEx( ComputerNamePhysicalDnsHostname ) ;
	if( name.empty() )
		name = nowide::getComputerNameEx( ComputerNameNetBIOS ) ;
	if( name.empty() )
		name = Environment::get( "COMPUTERNAME" , std::string() ) ;
	return name ;
}

