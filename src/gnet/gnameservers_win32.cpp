//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gnameservers_win32.cpp
///

#include "gdef.h"
#include "gnameservers.h"
#include "gstr.h"
#include "gstringview.h"
#include "gstringtoken.h"
#include "gscope.h"
#include "gbuffer.h"
#include "glog.h"
#include <iphlpapi.h>
#include <fstream>
#include <cstdlib>

std::vector<GNet::Address> GNet::nameservers( unsigned int port )
{
	std::vector<GNet::Address> result ;

	G::Buffer<char> info_buffer( sizeof(FIXED_INFO) ) ;
	FIXED_INFO * info = G::buffer_cast<FIXED_INFO*>( info_buffer ) ;

	ULONG size = sizeof(FIXED_INFO) ;
	auto rc = GetNetworkParams( info , &size ) ;
	if( rc == ERROR_BUFFER_OVERFLOW )
	{
		info_buffer.resize( size == ULONG(0) ? std::size_t(1U) : static_cast<std::size_t>(size) ) ;
		info = G::buffer_cast<FIXED_INFO*>( info_buffer ) ;
		rc = GetNetworkParams( info , &size ) ;
	}
	if( rc == NO_ERROR )
	{
		const char * p = info->DnsServerList.IpAddress.String ;
		if( GNet::Address::validStrings( p?p:"" , "0" ) )
			result.push_back( GNet::Address::parse( p?p:"" , port ) ) ;

		for( const IP_ADDR_STRING * addr = info->DnsServerList.Next ; addr ; addr = addr->Next )
		{
			p = addr->IpAddress.String ;
			if( GNet::Address::validStrings( p?p:"" , "0" ) )
				result.push_back( GNet::Address::parse( p?p:"" , port ) ) ;
		}
	}
	return result ;
}

