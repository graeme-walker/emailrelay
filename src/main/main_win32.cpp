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
// main_win32.cpp
//

#include "gdef.h"
#include "gsmtp.h"
#include "run.h"
#include "gappbase.h"
#include "gexception.h"
#include "resource.h"

namespace
{
	class App : public GGui::ApplicationBase
	{
	public:
		App( HINSTANCE h , HINSTANCE p , const char * name ) ;
		void onPaint( HDC hdc ) ;
		bool onCreate() ;
		DWORD windowStyle() const ;
	} ;
} ;

App::App( HINSTANCE h , HINSTANCE p , const char * name ) :
	GGui::ApplicationBase( h , p , name )
{
}

DWORD App::windowStyle() const
{
	return
		( GGui::Window::windowStylePopup() & ~WS_THICKFRAME ) |
		WS_MINIMIZEBOX ;
}

bool App::onCreate()
{
	GGui::Size size ;
	size.dx = size.dy = 100 ;
	resize( size ) ;
	return true ;
}

void App::onPaint( HDC hdc )
{
	int dx_window = internalSize().dx ;
	int dy_window = internalSize().dy ;
	int dx_icon = 32 ;
	int dy_icon = 32 ;

	int x = dx_window > dx_icon ? ((dx_window-dx_icon)/2) : 0 ;
	int y = dy_window > dy_icon ? ((dy_window-dy_icon)/2) : 0 ;

	::DrawIcon( hdc , x , y ,
		::LoadIcon(hinstance(),MAKEINTRESOURCE(IDI_ICON1)) ) ;
}

int WINAPI WinMain( HINSTANCE hinstance , HINSTANCE previous ,
	LPSTR command_line , int show )
{
	try
	{
		show = SW_MINIMIZE ;

		G::Arg arg ;
		arg.parse( hinstance , command_line ) ;

		App app( hinstance , previous , "E-MailRelay" ) ;

		try
		{
			Main::Run run( arg ) ;
			if( run.prepare() )
			{
				app.createWindow( show ) ;
				run.run() ;
			}
		}
		catch( std::exception & e )
		{
			app.messageBox( e.what() ) ;
		}

		return 0 ;
	}
	catch(...)
	{
	}
	return 1 ;
}

