//
// Copyright (C) 2001 Graeme Walker <graeme_walker@users.sourceforge.net>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
// ===
//
// gwinbase.cpp
//

#include "gdef.h"
#include "gwinbase.h"
#include "gdebug.h"
#include <windowsx.h>

GGui::WindowBase::WindowBase( HWND hwnd ) :
	m_hwnd(hwnd)
{
}

GGui::WindowBase::~WindowBase()
{
}

HWND GGui::WindowBase::handle() const
{
	return m_hwnd ;
}

GGui::WindowBase & GGui::WindowBase::operator=( const WindowBase & other )
{
	m_hwnd = other.m_hwnd ;
	return *this ;
}

GGui::WindowBase::WindowBase( const WindowBase &other ) :
	m_hwnd(other.m_hwnd)
{
}

void GGui::WindowBase::setHandle( HWND hwnd )
{
	m_hwnd = hwnd ;
}

GGui::Size GGui::WindowBase::internalSize() const
{
	Size size ;
	RECT rect ;
	if( ::GetClientRect( m_hwnd , &rect ) )
	{
		G_ASSERT( rect.right >= rect.left ) ;
		G_ASSERT( rect.bottom >= rect.top ) ;
		size.dx = rect.right - rect.left ;
		size.dy = rect.bottom - rect.top ;
	}
	return size ;
}

GGui::Size GGui::WindowBase::externalSize() const
{
	GGui::Size size ;
	RECT rect ;
	if( ::GetWindowRect( m_hwnd , &rect ) )
	{
		G_ASSERT( rect.right >= rect.left ) ;
		G_ASSERT( rect.bottom >= rect.top ) ;
		size.dx = rect.right - rect.left ;
		size.dy = rect.bottom - rect.top ;
	}
	return size ;
}

