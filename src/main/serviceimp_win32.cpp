//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file serviceimp_win32.cpp
///

#include "gdef.h"
#include "gnowide.h"
#include "serviceimp.h"
#include "servicecontrol.h"
#include "gprocess.h"
#include "gfile.h"
#include <fstream>

namespace ServiceImp
{
	ServiceMainFn m_service_main_fn ;
	HandlerFn m_handler_fn ;
	void WINAPI Handler( DWORD arg ) ;
	void WINAPI ServiceMainW( DWORD argc , wchar_t ** argv ) ;
}

void WINAPI ServiceImp::Handler( DWORD arg )
{
	if( ServiceImp::m_handler_fn )
		ServiceImp::m_handler_fn( arg ) ;
}

void WINAPI ServiceImp::ServiceMainW( DWORD argc , wchar_t ** argv )
{
	G::StringArray args ;
	for( DWORD i = 0 ; argv != nullptr && i < argc ; i++ )
	{
		if( argv[i] )
			args.push_back( G::Convert::narrow(std::wstring(argv[i])) ) ;
	}
	if( ServiceImp::m_service_main_fn )
		ServiceImp::m_service_main_fn( args ) ;
}

std::pair<std::string,DWORD> ServiceImp::install( const std::string & commandline , const std::string & name ,
	const std::string & display_name , const std::string & description )
{
	// see servicecontrol_win32.cpp
	return service_install( commandline , name , display_name , description , true ) ;
}

std::pair<std::string,DWORD> ServiceImp::remove( const std::string & service_name )
{
	// see servicecontrol_win32.cpp
	return service_remove( service_name ) ;
}

std::pair<ServiceImp::StatusHandle,DWORD> ServiceImp::statusHandle( const std::string & service_name , HandlerFn fn )
{
	m_handler_fn = fn ;
	StatusHandle h = G::nowide::registerServiceCtrlHandler( service_name , ServiceImp::Handler ) ;
	DWORD e = 0 ;
	if( h == HNULL )
		e = GetLastError() ;
	return { h , e } ;
}

DWORD ServiceImp::dispatch( ServiceMainFn service_main_fn )
{
	m_service_main_fn = service_main_fn ;
	bool ok = G::nowide::startServiceCtrlDispatcher( ServiceImp::ServiceMainW ) ;
	DWORD e = GetLastError() ;
	return ok ? DWORD(0) : e ;
}

DWORD ServiceImp::setStatus( StatusHandle hservice , DWORD new_state , DWORD timeout_ms , DWORD generic_error , DWORD specific_error ) noexcept
{
	SERVICE_STATUS s {} ;
	s.dwServiceType = SERVICE_WIN32_OWN_PROCESS ;
	s.dwCurrentState = new_state ;
	s.dwControlsAccepted = SERVICE_ACCEPT_STOP ;
	s.dwWin32ExitCode = NO_ERROR ;
	s.dwServiceSpecificExitCode = 0 ;
	if( generic_error )
	{
		s.dwWin32ExitCode = generic_error ;
		if( generic_error == ERROR_SERVICE_SPECIFIC_ERROR )
			s.dwServiceSpecificExitCode = specific_error ;
	}
	s.dwCheckPoint = 0 ;
	s.dwWaitHint = timeout_ms ;
	bool ok = !!SetServiceStatus( hservice , &s ) ;
	DWORD e = GetLastError() ;
	return ok ? DWORD(0) : e ;
}

void ServiceImp::log( const std::string & s ) noexcept
{
	static bool first = true ;
	static std::ofstream f ;
	try
	{
		if( first )
		{
			first = false ;
			HKEY hkey = HNULL ;
			G::nowide::regOpenKey( HKEY_LOCAL_MACHINE , G::Path("SOFTWARE")/G::Process::exe().withoutExtension().basename() , &hkey , true ) ;
			std::string logfile ;
			G::nowide::regGetValueString( hkey , "logfile" , &logfile ) ;
			if( !logfile.empty() )
				G::File::open( f , logfile ) ;
		}
		if( f.is_open() )
			f << s << "\r" << std::endl ;
	}
	catch(...)
	{
	}
}

