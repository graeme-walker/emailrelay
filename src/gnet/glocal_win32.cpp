//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file glocal_win32.cpp
///

#include "gdef.h"
#include "glocal.h"
#include "ghostname.h"
#include "gidn.h"
#include "gnowide.h"

std::string GNet::Local::hostname()
{
	std::string name = G::hostname() ; // ie. GetComputerNameEx(ComputerNamePhysicalDnsHostname)
	if( name.empty() )
		return "localhost" ;
	return name ;
}

std::string GNet::Local::canonicalName()
{
	static std::string result ;
	static bool first = true ;
	if( first )
	{
		first = false ;
		result = G::Idn::encode( G::nowide::getComputerNameEx( ComputerNameDnsFullyQualified ) ) ;
		if( result.empty() )
			result = G::Idn::encode(hostname()) + ".localnet" ;
	}
	return result ;
}

