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
// gappbase.cpp
//

#include "gdef.h"
#include "gappbase.h"
#include "gwindow.h"
#include "gpump.h"
#include "gdebug.h"
#include "glog.h"

GGui::ApplicationBase::ApplicationBase( HINSTANCE current , HINSTANCE previous ,
	const char *name ) :
		ApplicationInstance(current) ,
		m_previous(previous) ,
		m_name(name)
{
}

GGui::ApplicationBase::~ApplicationBase()
{
}

bool GGui::ApplicationBase::createWindow( int show_style , bool do_show )
{
	G_DEBUG( "GGui::ApplicationBase::createWindow" ) ;

	// first => register a window class
	if( m_previous == 0 )
	{
		if( !initFirst() )
		{
			G_DEBUG( "GGui::ApplicationBase::init: cannot register window class" ) ;
			//return false ;
		}
	}

	// create the main window
	if( !create( className() , title() , windowStyle() ,
			CW_USEDEFAULT , CW_USEDEFAULT , // position (x,y)
			CW_USEDEFAULT , CW_USEDEFAULT , // size
			NULL , // parent window
			NULL , // menu handle: 0 => use class's menu
			hinstance() ) )
	{
		G_DEBUG( "GGui::ApplicationBase::init: cannot create main window" ) ;
		return false ;
	}

	if( do_show )
	{
		show( show_style ) ;
		update() ;
	}

	return true ;
}

bool GGui::ApplicationBase::firstInstance() const
{
	return m_previous == 0 ;
}

bool GGui::ApplicationBase::initFirst()
{
	UINT id = resource() ;

	G_DEBUG( "GGui::ApplicationBase::initFirst: loading main icon: id " << id ) ;

	HICON icon = id ? ::LoadIcon(hinstance(),MAKEINTRESOURCE(resource())) : 0 ;

	G_DEBUG( "GGui::ApplicationBase::initFirst: "
		<< "registering main window class \"" << className()
		<< "\", hinstance " << hinstance() ) ;

	return registerWindowClass( className() ,
		hinstance() ,
		classStyle() ,
		icon ? icon : GGui::Window::classIcon() ,
		GGui::Window::classCursor() ,
		backgroundBrush() ,
		MAKEINTRESOURCE(resource()) ) ;
}

void GGui::ApplicationBase::close() const
{
	::SendMessage( handle() , WM_CLOSE , 0 , 0 ) ;
}

void GGui::ApplicationBase::run( bool with_idle )
{
	if( with_idle )
		GGui::Pump::run( handle() , GGui::Cracker::wm_idle() ) ;
	else
		GGui::Pump::run() ;
}

void GGui::ApplicationBase::onDestroy()
{
	GGui::Pump::quit() ;
}

const char * GGui::ApplicationBase::title() const
{
	return m_name.c_str() ;
}

const char * GGui::ApplicationBase::className() const
{
	return m_name.c_str() ;
}

HBRUSH GGui::ApplicationBase::backgroundBrush()
{
	return GGui::Window::classBrush() ;
}

DWORD GGui::ApplicationBase::windowStyle() const
{
	return GGui::Window::windowStyleMain() ;
}

DWORD GGui::ApplicationBase::classStyle() const
{
	return GGui::Window::classStyle() ;
}

UINT GGui::ApplicationBase::resource() const
{
	return 0 ;
}

void GGui::ApplicationBase::beep() const
{
	::MessageBeep( MB_ICONEXCLAMATION ) ;
}

void GGui::ApplicationBase::messageBox( const std::string & message )
{
	HWND box_parent = ::GetActiveWindow() ; // eg. a dialog box
	if( box_parent == NULL )
		box_parent = handle() ;

	::MessageBox( box_parent , message.c_str() , title() ,
		MB_OK | MB_ICONEXCLAMATION ) ;
}

//static
void GGui::ApplicationBase::messageBox( const std::string & title , const std::string & message )
{
	HWND box_parent = NULL ;
	::MessageBox( box_parent , message.c_str() , title.c_str() ,
		MB_OK | MB_ICONEXCLAMATION ) ;
}


