//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gwinbase.cpp
///

#include "gdef.h"
#include "gnowide.h"
#include "gstr.h"
#include "gwinbase.h"
#include "glog.h"
#include "gassert.h"
#include <windowsx.h>
#include <vector>
#include <cstring>

GGui::WindowBase::WindowBase( HWND hwnd ) :
	m_hwnd(hwnd)
{
}

GGui::WindowBase::~WindowBase()
= default ;

void GGui::WindowBase::setHandle( HWND hwnd ) noexcept
{
	m_hwnd = hwnd ;
}

GGui::Size GGui::WindowBase::internalSize() const
{
	RECT rect ;
	if( GetClientRect( m_hwnd , &rect ) )
	{
		G_ASSERT( rect.left == 0 ) ;
		G_ASSERT( rect.top == 0 ) ;
		return Size( rect.right , rect.bottom ) ; // NOLINT(*-return-braced-init-list)
	}
	else
	{
		return {} ;
	}
}

GGui::Size GGui::WindowBase::externalSize() const
{
	RECT rect ;
	if( GetWindowRect( m_hwnd , &rect ) )
	{
		G_ASSERT( rect.right >= rect.left ) ;
		G_ASSERT( rect.bottom >= rect.top ) ;
		return Size( rect.right - rect.left , rect.bottom - rect.top ) ; // NOLINT(*-return-braced-init-list)
	}
	else
	{
		return {} ;
	}
}

std::string GGui::WindowBase::windowClass() const
{
	return G::nowide::getClassName( m_hwnd ) ;
}

HINSTANCE GGui::WindowBase::windowInstanceHandle() const
{
	return reinterpret_cast<HINSTANCE>(G::nowide::getWindowLongPtr(m_hwnd,GWLP_HINSTANCE)) ;
}

