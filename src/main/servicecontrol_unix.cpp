//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file servicecontrol_unix.cpp
///

#include "servicecontrol.h"
#include <string>

std::pair<std::string,DWORD> service_install( const std::string & , const std::string & , const std::string & ,
	const std::string & , bool )
{
	return {{},0} ;
}

bool service_installed( const std::string & )
{
	return true ;
}

std::pair<std::string,DWORD> service_remove( const std::string & )
{
	return {"not implemented",1} ;
}

std::pair<std::string,DWORD> service_start( const std::string & )
{
	return {"not implemented",1} ;
}

