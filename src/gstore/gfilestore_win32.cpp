//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gfilestore_win32.cpp
///

#include "gdef.h"
#include "gfilestore.h"
#include "gpath.h"
#include "genvironment.h"
#include <cstdio>
#include <crtdbg.h>

namespace GStore
{
	namespace FileStoreImp
	{
		struct NoCheck
		{
			NoCheck() ;
			~NoCheck() ;
			NoCheck( const NoCheck & ) = delete ;
			NoCheck( NoCheck && ) = delete ;
			NoCheck & operator=( const NoCheck & ) = delete ;
			NoCheck & operator=( NoCheck && ) = delete ;
			_invalid_parameter_handler m_handler ;
			int m_mode ;
			static void handler( const wchar_t * , const wchar_t * , const wchar_t * , unsigned int , uintptr_t ) ;
		} ;
	}
}

G::Path GStore::FileStore::defaultDirectory()
{
	return G::Path(G::Environment::get("ProgramData","c:/ProgramData"))/"E-MailRelay"/"spool" ;
}

void GStore::FileStore::osinit()
{
	constexpr int limit = 8192 ;
	if( _getmaxstdio() < limit )
	{
		FileStoreImp::NoCheck no_check ;
		_setmaxstdio( limit ) ;
	}
}

GStore::FileStoreImp::NoCheck::NoCheck() :
	m_handler(_set_invalid_parameter_handler(NoCheck::handler)) ,
	m_mode(_CrtSetReportMode(_CRT_ASSERT,0))
{
}

GStore::FileStoreImp::NoCheck::~NoCheck()
{
	_set_invalid_parameter_handler( m_handler ) ;
	_CrtSetReportMode( _CRT_ASSERT , m_mode ) ;
}

void GStore::FileStoreImp::NoCheck::handler( const wchar_t * , const wchar_t * , const wchar_t * , unsigned int , uintptr_t )
{
	// no-op
}

