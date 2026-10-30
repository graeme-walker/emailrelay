//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file serviceimp_none.cpp
///
// A do-nothing implementation that might be useful when testing the
// service wrapper without a service manager, eg. with Wine.
//

#include "gdef.h"
#include "serviceimp.h"
#include "servicecontrol.h"
#include "gconvert.h"
#include "gstringview.h"
#include <iostream>

namespace ServiceImp
{
	std::string m_name ;
	constexpr DWORD SERVICE_STOPPED = 1 ; // see winsvc.h
	constexpr DWORD SERVICE_START_PENDING = 2 ;
	constexpr DWORD SERVICE_STOP_PENDING = 3 ;
	constexpr DWORD SERVICE_RUNNING = 4 ;
}

std::pair<std::string,DWORD> ServiceImp::install( const std::string & , const std::string & name ,
	const std::string & , const std::string & )
{
	std::cout << "ServiceImp::install: " << name << std::endl ;
	m_name = name ;
	return {{},0} ;
}

std::pair<std::string,DWORD> ServiceImp::remove( const std::string & name )
{
	std::cout << "ServiceImp::remove: " << name << std::endl ;
	return {{},0} ;
}

std::pair<ServiceImp::StatusHandle,DWORD> ServiceImp::statusHandle( const std::string & , HandlerFn )
{
	return {1,0} ;
}

DWORD ServiceImp::dispatch( ServiceMainFn fn )
{
	sleep( 1 ) ;
	fn( {m_name} ) ;
	while( true )
		sleep( 1 ) ;
	return 0 ;
}

DWORD ServiceImp::setStatus( StatusHandle , DWORD new_state , DWORD /*timeout*/ , DWORD /*generic_error*/ , DWORD /*specific_error*/ ) noexcept
{
	std::string_view sv ;
	if( new_state == SERVICE_STOPPED ) sv = "stopped" ;
	if( new_state == SERVICE_START_PENDING ) sv = "start-pending" ;
	if( new_state == SERVICE_STOP_PENDING ) sv = "stop-pending" ;
	if( new_state == SERVICE_RUNNING ) sv = "running" ;
	std::cout << "ServiceImp::setStatus: " << new_state << " " << sv << std::endl ;
	return 0 ;
}

void ServiceImp::log( const std::string & s ) noexcept
{
	std::cout << s << std::endl ;
}

