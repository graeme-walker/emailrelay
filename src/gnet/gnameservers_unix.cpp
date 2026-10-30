//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gnameservers_unix.cpp
///

#include "gdef.h"
#include "gnameservers.h"
#include "gstr.h"
#include "gstringview.h"
#include "gstringtoken.h"
#include <fstream>

std::vector<GNet::Address> GNet::nameservers( unsigned int port )
{
	std::vector<GNet::Address> result ;
	std::string line ;
	std::ifstream f( "/etc/resolv.conf" ) ;
	while( G::Str::readLine( f , line ) )
	{
		std::string_view sv( line ) ;
		G::StringTokenView t( sv , " \t" ) ;
		if( t.valid() && G::Str::imatch(t(),"nameserver") )
		{
			++t ;
			if( t.valid() && GNet::Address::validStrings(G::sv_to_string(t()),"0") )
				result.push_back( GNet::Address::parse( G::sv_to_string(t()) , port ) ) ;
		}
	}
	return result ;
}

