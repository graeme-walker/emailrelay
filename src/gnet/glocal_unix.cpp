//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file glocal_unix.cpp
///

#include "gdef.h"
#include "glocal.h"
#include "ghostname.h"
#include "gidn.h"
#include "gresolver.h"

std::string GNet::Local::hostname()
{
	std::string name = G::hostname() ;
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
		std::string name = G::Idn::encode( hostname() ) ;
		Location location( name.append(":0") ) ;
		auto pair = Resolver::resolve( location , Resolver::Config().set_with_canonical_name().set_raw() ) ;
		bool ok = pair.first.empty() ;
		if( ok && !pair.second.empty() )
			result = G::Idn::encode( pair.second ) ;
		else
			result = name + ".localnet" ;
	}
	return result ;
}

