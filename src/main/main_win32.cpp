//
// Copyright (C) 2001-2002 Graeme Walker <graeme_walker@users.sourceforge.net>
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
// This file contains all the GUI code
// for the Windows executable. (There's too much
// in here, but at least it keeps it out of sight
// when in a unix mindset.)
//

#include "gdef.h"
#include "gsmtp.h"
#include "commandline.h"
#include "configuration.h"
#include "legal.h"
#include "resource.h"
#include "run.h"
#include "gappbase.h"
#include "gassert.h"
#include "gcontrol.h"
#include "gdialog.h"
#include "gexception.h"
#include "glog.h"
#include "gmd5.h"
#include "gmemory.h"
#include "gmessagestore.h"
#include "gmonitor.h"
#include "gnoncopyable.h"
#include "gpump.h"
#include "gregistry.h"
#include "gstoredmessage.h"
#include "gstr.h"
#include "gtray.h"
#include <algorithm>
#include <list>

namespace
{
	class App ;

	class Form : public GGui::Dialog
	{
	public:
		Form( App & , const Main::Configuration & cfg , bool confirm ) ;
		void close() ;
	private:
		virtual bool onInit() ;
		virtual void onNcDestroy() ;
		virtual void onCommand( unsigned int id ) ;
		std::string text() const ;
	private:
		App & m_app ;
		GGui::EditBox m_edit_box ;
		Main::Configuration m_cfg ;
		bool m_confirm ;
	} ;

	class App : public GGui::ApplicationBase
	{
	public:
		G_EXCEPTION( Error , "application error" ) ;
		App( HINSTANCE h , HINSTANCE p , const char * name ) ;
		void init( const Main::Configuration & cfg ) ;
		void setStatus( const std::string & , const std::string & ) ;
		bool confirm() ;
		void formOk() ;
		void formDone() ;
	private:
		void doOpen() ;
		void doClose() ;
		void doQuit() ;
		void hide() ;
		virtual UINT resource() const ;
		virtual DWORD windowStyle() const ;
		virtual bool onCreate() ;
		virtual bool onClose() ;
		virtual void onTrayDoubleClick() ;
		virtual void onTrayRightMouseButtonDown() ;
		virtual void onDimension( int & , int & ) ;
		virtual bool onSysCommand( SysCommand ) ;
		virtual LRESULT onUser( WPARAM , LPARAM ) ;
	private:
		std::auto_ptr<GGui::Tray> m_tray ;
		std::auto_ptr<Form> m_form ;
		std::auto_ptr<Main::Configuration> m_cfg ;
		bool m_quit ;
		bool m_use_tray ;
		unsigned int m_icon ;
		bool m_external_gui ;
	} ;

	class Menu
	{
	public:
		G_EXCEPTION( Error , "menu error" ) ;
		explicit Menu( unsigned int resource_id ) ;
		~Menu() ;
		int popup( const GGui::WindowBase & w , bool with_open , bool with_close ) ;
	private:
		HMENU m_hmenu ;
		HMENU m_hmenu_popup ;
		Menu( const Menu & ) ;
		void operator=( const Menu & ) ;
	} ;

	class Run : public Main::Run
	{
	public:
		Run( App & app , const G::Arg & ) ;
	protected:
		virtual void onEvent( const std::string & , const std::string & , const std::string & ) ;
		virtual bool runnable() const ;
	private:
		App & m_app ;
		bool m_runnable ;
	} ;
} ;

// ===

Form::Form( App & app , const Main::Configuration & cfg , bool confirm ) :
	GGui::Dialog(app) ,
	m_app(app) ,
	m_cfg(cfg) ,
	m_edit_box(*this,IDC_EDIT1) ,
	m_confirm(confirm)
{
}

bool Form::onInit()
{
	m_edit_box.set( text() ) ;
	return true ;
}

std::string Form::text() const
{
	const std::string crlf( "\r\n" ) ;

	std::ostringstream ss ;
	ss
		<< "E-MailRelay V" << Main::Run::versionNumber() << crlf << crlf
		<< "Configuration..." << crlf
		<< m_cfg.str("* ",crlf)
		;

	if( GNet::Monitor::instance() )
	{
		ss << crlf << "Network connections..." << crlf ;
		GNet::Monitor::instance()->report( ss , "* " , crlf ) ;
	}

	ss
		<< crlf << Main::Legal::warranty("",crlf)
		<< crlf << Main::Legal::copyright() ;

	return ss.str() ;
}

void Form::close()
{
	end() ;
}

void Form::onNcDestroy()
{
	m_app.formDone() ;
}

void Form::onCommand( unsigned int id )
{
	if( id == IDOK && ( !m_confirm || m_app.confirm() ) )
	{
		m_app.formOk() ;
		end() ;
	}
}

// ===

App::App( HINSTANCE h , HINSTANCE p , const char * name ) :
	GGui::ApplicationBase( h , p , name ) ,
	m_use_tray(false) ,
	m_quit(false) ,
	m_icon(0U) ,
	m_external_gui(false)
{
}

void App::init( const Main::Configuration & cfg )
{
	m_use_tray = cfg.daemon() ;
	m_cfg <<= new Main::Configuration(cfg) ;
	m_icon = m_cfg->icon() % 4U ;
}

void App::onDimension( int & dx , int & dy )
{
	G_ASSERT( m_form.get() != NULL ) ;
	if( m_form.get() )
	{
		// (force main window's internal size to be
		// the same size as the form, but x and y
		// are the window's external size)

		const bool has_menu = false ;
		GGui::Size size = m_form->externalSize() ;
		GGui::Size border = GGui::Window::borderSize(has_menu) ;
		dx = size.dx + border.dx ;
		dy = size.dy + border.dy ;
	}
}

void App::hide()
{
	show( SW_HIDE ) ;
}

DWORD App::windowStyle() const
{
	return GGui::Window::windowStyleMain() ;
}

UINT App::resource() const
{
	// (resource() provides the combined menu and icon id, but we have no menus)
	if( m_icon == 0U ) return IDI_ICON1 ;
	if( m_icon == 1U ) return IDI_ICON2 ;
	if( m_icon == 2U ) return IDI_ICON3 ;
	G_ASSERT( m_icon == 3U ) ; return IDI_ICON4 ;
}

bool App::onCreate()
{
	if( m_use_tray )
		m_tray <<= new GGui::Tray(resource(),*this,"E-MailRelay") ;
	else
		doOpen() ;
	return true ;
}

void App::onTrayRightMouseButtonDown()
{
	Menu menu( IDR_MENU1 ) ;
	bool form_is_open = m_form.get() != NULL ;
	bool with_open = m_external_gui || !form_is_open ;
	bool with_close = m_external_gui || form_is_open ;
	int id = menu.popup( *this , with_open , with_close ) ;

	// make it asychronous to prevent "RPC_E_CANTCALLOUT_ININPUTSYNCCALL" --
	// see App::onUser()
	::PostMessage( handle() , wm_user() , 0 , static_cast<LPARAM>(id) ) ;
}

void App::onTrayDoubleClick()
{
	// make it asychronous to prevent "RPC_E_CANTCALLOUT_ININPUTSYNCCALL" --
	// see App::onUser()
	::PostMessage( handle() , wm_user() , 0 , static_cast<LPARAM>(IDM_OPEN) ) ;
}

LRESULT App::onUser( WPARAM , LPARAM lparam )
{
	int id = static_cast<int>(lparam) ;
	if( id == IDM_OPEN ) doOpen() ;
	if( id == IDM_CLOSE ) doClose() ;
	if( id == IDM_QUIT ) doQuit() ;
	return 0L ;
}

void App::doOpen()
{
	if( !m_external_gui )
	{
		if( m_form.get() == NULL )
		{
			m_form <<= new Form( *this , *m_cfg.get() , !m_use_tray ) ;
			if( ! m_form->runModeless(IDD_DIALOG1) )
				throw Error( "cannot run dialog box" ) ;
		}

		resize( externalSize() ) ; // no-op in itself, but uses onDimension()
		show() ;
	}
}

void App::doQuit()
{
	m_quit = true ;
	close() ; // triggers onClose(), but without doClose()
}

bool App::onClose()
{
	// (this is triggered by close() or using the system close menu item)

	bool really_quit = m_quit || ( !m_use_tray && confirm() ) ;
	if( !really_quit ) doClose() ;
	return really_quit ;
}

bool App::confirm()
{
	return messageBoxQuery("Really quit?") ;
}

void App::doClose()
{
	hide() ;

	// (close the form so that it gets recreated each time with current data)
	if( m_form.get() != NULL )
		m_form->close() ;
}

void App::formOk()
{
	// (this is triggered by clicking the OK button)
	m_use_tray ? doClose() : doQuit() ;
}

void App::formDone()
{
	// (this is called from Form::onNcDestroy)
	m_form <<= 0 ;
}

bool App::onSysCommand( SysCommand sc )
{
	// true <= processed as no-op => dont change size
	return sc == scMaximise || sc == scSize ;
}

void App::setStatus( const std::string & s1 , const std::string & s2 )
{
	std::string message( title() ) ;
	if( !s1.empty() ) message.append( std::string(": ")+s1 ) ;
	if( !s2.empty() ) message.append( std::string(": ")+s2 ) ;
	::SetWindowText( handle() , message.c_str() ) ;
}

// ===

Menu::Menu( unsigned int id )
{
	HINSTANCE hinstance = GGui::ApplicationInstance::hinstance() ;
	m_hmenu = ::LoadMenu( hinstance , MAKEINTRESOURCE(id) ) ;
	if( m_hmenu == NULL )
		throw Error() ;
}

int Menu::popup( const GGui::WindowBase & w , bool with_open , bool with_close )
{
	const int open_pos = 0 ;
	const int close_pos = 1 ;

	POINT p ;
	::GetCursorPos( &p ) ;
	::SetForegroundWindow( w.handle() ) ;

	// TrackPopup() only works with a sub-menu, although
	// you would never guess from the documentation
	//
	m_hmenu_popup = ::GetSubMenu( m_hmenu , 0 ) ;

	// make the "open" menu item bold
	//
	::SetMenuDefaultItem( m_hmenu_popup , open_pos , TRUE ) ;

	// optionally grey-out menu items
	//
	if( !with_open )
		::EnableMenuItem( m_hmenu_popup , open_pos , MF_BYPOSITION | MF_GRAYED ) ;
	if( !with_close )
		::EnableMenuItem( m_hmenu_popup , close_pos , MF_BYPOSITION | MF_GRAYED ) ;

	// display the menu
	//
	BOOL rc = ::TrackPopupMenuEx( m_hmenu_popup ,
		TPM_RETURNCMD , p.x , p.y , w.handle() , NULL ) ;
	return static_cast<int>(rc) ; // BOOL->int!, only in Microsoft wonderland
}

Menu::~Menu()
{
	if( m_hmenu != NULL )
		::DestroyMenu( m_hmenu ) ;
}

// ===

Run::Run( App & app , const G::Arg & arg ) :
	Main::Run(arg) ,
	m_app(app) ,
	m_runnable(true)
{
}

void Run::onEvent( const std::string & category , const std::string & s1 , const std::string & s2 )
{
	if( category == "client" )
		m_app.setStatus( s1 , s2 ) ;
}

bool Run::runnable() const
{
	return m_runnable ;
}

// ===

int WINAPI WinMain( HINSTANCE hinstance , HINSTANCE previous ,
	LPSTR command_line , int show )
{
	try
	{
		G::Arg arg ;
		arg.parse( hinstance , command_line ) ;
		App app( hinstance , previous , "E-MailRelay" ) ;

		try
		{
			Run run( app , arg ) ;
			if( run.prepare() )
			{
				const bool visible = ! run.cfg().daemon() ;
				app.init( run.cfg() ) ;
				app.createWindow( show , visible ) ;
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
		::MessageBeep( MB_ICONHAND ) ;
	}
	return 1 ;
}

