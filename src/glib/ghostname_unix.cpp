//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file ghostname_unix.cpp
///

#include "gdef.h"
#include "ghostname.h"
#include "gstr.h"
#include "genvironment.h"
#include <sys/utsname.h>

std::string G::hostname()
{
	struct utsname info {} ;
	int rc = ::uname( &info ) ;
	if( rc == -1 )
		return {} ;

	info.nodename[sizeof(info.nodename)-1U] = '\0' ;
	std::string_view name = G::Str::headView( info.nodename , {".",1U} , false ) ;

	if( name.empty() ) // pathologically "uname -n" can be empty
		return Environment::get( "HOSTNAME" , std::string() ) ;

	return Str::printable( name ) ;
}

