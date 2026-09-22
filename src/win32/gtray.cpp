//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gtray.cpp
///

#include "gdef.h"
#include "gnowide.h"
#include "gtray.h"
#include "gstr.h"
#include "gappinst.h"
#include "gassert.h"
#include <cstring>

GGui::Tray::Tray( unsigned int icon_id , const WindowBase & window ,
	const std::string & tip , unsigned int message )
{
	m_info = G::nowide::NOTIFYICONDATA_type {} ;
	G_ASSERT( m_info.uVersion == 0 ) ; // winxp

	m_info.cbSize = sizeof(m_info) ;
	m_info.hWnd = window.handle() ;
	m_info.uID = message ;
	m_info.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP ;
	m_info.uCallbackMessage = message ;

	int rc = G::nowide::shellNotifyIcon( ApplicationInstance::hinstance() , NIM_ADD , &m_info , icon_id , tip ) ;
	if( rc == 2 )
		throw IconError() ;
	else if( rc == 1 )
		throw Error() ;
}

GGui::Tray::~Tray()
{
	static_assert( noexcept(G::nowide::shellNotifyIcon(NIM_DELETE,&m_info,std::nothrow)) , "" ) ;
	m_info.uFlags = 0 ;
	m_info.uCallbackMessage = 0 ;
	m_info.hIcon = HNULL ;
	G::nowide::shellNotifyIcon( NIM_DELETE , &m_info , std::nothrow ) ;
}

