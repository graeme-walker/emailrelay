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
// gwindow.cpp
//

#include "gdef.h"
#include "gwindow.h"
#include "gdebug.h"
#include "glog.h"

GGui::Window::Window( HWND hwnd ) :
	Cracker(hwnd)
{
	G_DEBUG( "GGui::Window::ctor: this = " << this ) ;
}

GGui::Window::~Window()
{
}

extern "C" LRESULT CALLBACK gwindow_wndproc_export( HWND hwnd , UINT message , WPARAM wparam , LPARAM lparam )
{
	return GGui::Window::wndProc( hwnd , message , wparam , lparam ) ;
}

WNDPROC GGui::Window::windowProcedure()
{
	return gwindow_wndproc_export ;
}

bool GGui::Window::registerWindowClass( const char *class_name ,
	HINSTANCE hinstance , UINT style ,
	HICON icon , HCURSOR cursor ,
	HBRUSH background , const char *menu_name )
{
	WNDCLASS c ;

	c.style = style ;
	c.lpfnWndProc = gwindow_wndproc_export ;
	c.cbClsExtra = 4 ; // reserved
	c.cbWndExtra = 4 ; // for pointer to Window
	c.hInstance = hinstance ;
	c.hIcon = icon ;
	c.hCursor = cursor ;
	c.hbrBackground = background ;
	c.lpszMenuName = menu_name ;
	c.lpszClassName = class_name ;

	return ::RegisterClass( &c ) != 0 ;
}

bool GGui::Window::create( const char *class_name ,
	const char *title , DWORD style ,
	int x , int y , int dx , int dy ,
	HWND parent , HMENU menu , HINSTANCE hinstance )
{
	G_ASSERT( m_hwnd == NULL ) ;

	if( title == NULL )
		title = "" ;

	void *vp = reinterpret_cast<void*>(this) ;
	m_hwnd = ::CreateWindow( class_name , title ,
		style , x , y , dx , dy ,
		parent , menu , hinstance , vp ) ;

	G_DEBUG( "GGui::Window::create: handle " << m_hwnd ) ;
	return m_hwnd != NULL ;
}

void GGui::Window::update()
{
	G_ASSERT( m_hwnd != NULL ) ;
	::UpdateWindow( m_hwnd ) ;
}

void GGui::Window::show( int style )
{
	G_ASSERT( m_hwnd != NULL ) ;
	::ShowWindow( m_hwnd , style ) ;
}

#if 0
void GGui::Window::setGlobal()
{
	G_ASSERT( m_hwnd != NULL ) ;
	G_ASSERT( sizeof(LONG) >= sizeof(HWND) ) ;
	LONG cl = reinterpret_cast<LONG>(m_hwnd) ;
	::SetClassLong( m_hwnd , 0 , cl ) ;
}
HWND GGui::Window::getGlobal() const
{
	G_ASSERT( m_hwnd != NULL ) ;
	LONG cl = ::GetClassLong( m_hwnd , 0 ) ;
	return reinterpret_cast<HWND>(cl) ;
}
#endif

void GGui::Window::invalidate( bool erase )
{
	::InvalidateRect( m_hwnd , NULL , erase ) ;
}

GGui::Window * GGui::Window::instance( HWND hwnd )
{
	G_ASSERT( hwnd != NULL ) ;
	LONG wl = ::GetWindowLong( hwnd , 0 ) ;
	void far * vp = reinterpret_cast<void far *>(wl) ;
	return reinterpret_cast<Window*>(vp) ;
}

//static
LRESULT GGui::Window::wndProc( HWND hwnd , UINT msg , WPARAM wparam , LPARAM lparam )
{
	if( msg == WM_CREATE )
	{
		G_DEBUG( "GGui::Window::wndProc: WM_CREATE: " << hwnd ) ;
		CREATESTRUCT *cs = reinterpret_cast<CREATESTRUCT *>(lparam) ;
		Window *window = reinterpret_cast<Window *>(cs->lpCreateParams) ;
		G_ASSERT( window != NULL ) ;
		G_DEBUG( "GGui::Window::wndProc: WM_CREATE: this = " << window ) ;
		void far * vp = reinterpret_cast<void far *>(window) ;
		LONG wl = reinterpret_cast<LONG>(vp) ;
		::SetWindowLong( hwnd , 0 , wl ) ;
		window->setHandle( hwnd ) ;
		return window->onCreateCore() ? 0 : -1 ;
	}
	else
	{
		Window *window = Window::instance( hwnd ) ;
		// G_ASSERT( window != NULL ) ;
		if( window == NULL )
			return ::DefWindowProc(hwnd,msg,wparam,lparam) ;

		G_ASSERT( hwnd == window->handle() ) ;
		return window->wndProc( msg , wparam , lparam ) ;
	}
}

LRESULT GGui::Window::sendUserString( HWND hwnd , const char *string )
{
	G_ASSERT( string != NULL ) ;
	return ::SendMessage( hwnd , WM_USER_STRING , 0 , (LPARAM)(const char far *)string ) ;
}

LRESULT GGui::Window::onUserString( const char * )
{
	return 0 ;
}

LRESULT GGui::Window::wndProc( unsigned message , WPARAM wparam , LPARAM lparam )
{
	if( message == WM_USER_STRING )
	{
		G_DEBUG( "GGui::Window::onUserString" ) ;
		if( lparam )
		{
			std::string s( (const char far *)lparam ) ; // far to near
			return onUserString( s.c_str() ) ;
		}
		else
		{
			return onUserString( "" ) ;
		}
	}
	else
	{
		bool defolt ;
		LRESULT rc = wndProcCore( message , wparam , lparam , defolt ) ;
		if( defolt )
			return ::DefWindowProc( m_hwnd , message , wparam , lparam ) ;
		else
			return rc ;
	}
}

LRESULT GGui::Window::wndProcCore( unsigned msg , WPARAM wparam , LPARAM lparam ,
	bool & defolt )
{
	try
	{
		return crack( msg , wparam , lparam , defolt ) ;
	}
	catch( std::exception & e )
	{
		onException( e ) ;
		return 0 ;
	}
}

bool GGui::Window::onCreateCore()
{
	try
	{
		return onCreate() ;
	}
	catch( std::exception & e )
	{
		onException( e ) ;
		return false ;
	}
}

void GGui::Window::destroy()
{
	::DestroyWindow( handle() ) ;
}

DWORD GGui::Window::windowStyleMain()
{
	return WS_OVERLAPPEDWINDOW ;
}

DWORD GGui::Window::windowStylePopup()
{
	return
		WS_THICKFRAME |
		WS_POPUP |
		WS_SYSMENU |
		WS_CAPTION |
		WS_VISIBLE ;
}

DWORD GGui::Window::windowStyleChild()
{
	return WS_CHILDWINDOW ;
}

UINT GGui::Window::classStyle( bool redraw )
{
	return
		redraw ?
			(CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW) :
			(CS_DBLCLKS) ;
}

HBRUSH GGui::Window::classBrush()
{
	return (HBRUSH)( 1 + COLOR_BACKGROUND ) ;
}

HICON GGui::Window::classIcon()
{
	return ::LoadIcon( NULL , IDI_APPLICATION ) ;
}

HCURSOR GGui::Window::classCursor()
{
	return ::LoadCursor( NULL , IDC_ARROW ) ;
}

GGui::Size GGui::Window::borderSize( bool has_menu )
{
	// note that the Win3.1 and Win32 help files
	// have different definitions of these metrics
	// wrt. the +/- CYBORDER -- needs more testing

	// See also AdjustWindowRect, AdjustWindowRectEx and
	// MSDN 4 "Ask Dr GUI #10".

	Size size ;
	size.dx = ::GetSystemMetrics( SM_CXFRAME ) * 2 ;
	size.dy = ::GetSystemMetrics( SM_CYFRAME ) * 2 ;
	size.dy += ::GetSystemMetrics( SM_CYCAPTION ) - ::GetSystemMetrics( SM_CYBORDER ) ;
	if( has_menu )
		size.dy += ::GetSystemMetrics( SM_CYMENU ) + ::GetSystemMetrics( SM_CYBORDER ) ;
	return size ;
}

void GGui::Window::resize( Size new_size , bool repaint )
{
	// note that GetWindowRect() returns coordinates
	// relative to the top left corner of the screen --
	// MoveWindow takes coordinates relative to the
	// screen for top-level windows, but relative to
	// the parent window for child windows

	RECT rect ;
	if( ::GetWindowRect( m_hwnd , &rect ) )
	{
		HWND parent = ::GetParent( handle() ) ;
		const bool child_window = parent != 0 ;
		if( child_window )
		{
			rect.left = 0 ;
			rect.top = 0 ;
		}

		::MoveWindow( handle() ,
			rect.left ,
			rect.top ,
			new_size.dx ,
			new_size.dy ,
			repaint ) ;
	}
}

void GGui::Window::onException( std::exception & e )
{
	// it is not clear that it is safe to throw out of a
	// window procedure, but it seems to work okay
	//
	throw ;
}

