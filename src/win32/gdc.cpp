//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gdc.cpp
///

#include "gdef.h"
#include "gdc.h"

GGui::DeviceContext::DeviceContext( HWND hwnd ) :
	m_hwnd(hwnd) ,
	m_do_release(true)
{
	m_hdc = ::GetDC( m_hwnd ) ;
}

GGui::DeviceContext::DeviceContext( HDC hdc ) :
	m_hdc(hdc) ,
	m_hwnd(HNULL) ,
	m_do_release(false) // ignored
{
}

GGui::DeviceContext::~DeviceContext()
{
	if( m_hwnd != HNULL && m_do_release )
		::ReleaseDC( m_hwnd , m_hdc ) ;
}

HDC GGui::DeviceContext::handle() const
{
	return m_hdc ;
}

HDC GGui::DeviceContext::extractHandle()
{
	m_do_release = false ;
	return m_hdc ;
}

HDC GGui::DeviceContext::operator()() const
{
	return m_hdc ;
}

void GGui::DeviceContext::swapBuffers()
{
	BOOL ok = ::SwapBuffers( m_hdc ) ;
	GDEF_IGNORE_VARIABLE( ok ) ;
}

// ===

GGui::ScreenDeviceContext::ScreenDeviceContext() :
	m_dc(::CreateDCA("DISPLAY",nullptr,nullptr,nullptr))
{
}

GGui::ScreenDeviceContext::~ScreenDeviceContext()
{
	::DeleteDC( m_dc ) ;
}

HDC GGui::ScreenDeviceContext::handle()
{
	return m_dc ;
}

HDC GGui::ScreenDeviceContext::operator()()
{
	return handle() ;
}

int GGui::ScreenDeviceContext::colours() const
{
	return ::GetDeviceCaps( m_dc , NUMCOLORS ) ;
}

int GGui::ScreenDeviceContext::dx() const
{
	return ::GetDeviceCaps( m_dc , HORZRES ) ;
}

int GGui::ScreenDeviceContext::dy() const
{
	return ::GetDeviceCaps( m_dc , VERTRES ) ;
}

int GGui::ScreenDeviceContext::aspectx() const
{
	return ::GetDeviceCaps( m_dc , ASPECTX ) ;
}

int GGui::ScreenDeviceContext::aspecty() const
{
	return ::GetDeviceCaps( m_dc , ASPECTY ) ;
}

