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
#include "configuration.h"
#include "commandline.h"
#include "resource.h"
#include "gtray.h"
#include "gappbase.h"
#include "gdialog.h"
#include "gcontrol.h"
#include "gmonitor.h"
#include "gpump.h"
#include "gstr.h"
#include "gexception.h"
#include "gmemory.h"
#include "glog.h"
#include "glogoutput.h"

namespace
{
	class Callback
	{
		public: virtual void callback() = 0 ;
	} ;
	class Form : public GGui::Dialog
	{
	public:
		Form( GGui::ApplicationBase & , Callback & , const Main::Configuration & cfg ) ;
		void close() ;
		void set( const std::string & text ) ;
	private:
		virtual bool onInit() ;
		virtual void onNcDestroy() ;
		virtual void onClose() ;
	private:
		Callback & m_callback ;
		GGui::EditBox m_edit_box ;
		Main::Configuration m_cfg ;
	} ;
	class App : public GGui::ApplicationBase , public Callback
	{
	public:
		G_EXCEPTION( Error , "application error" ) ;
		App( HINSTANCE h , HINSTANCE p , const char * name ) ;
		void init( const Main::Configuration & cfg ) ;
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
		virtual void callback() ;
		virtual void onDimension( int & , int & ) ;
		virtual bool onSysCommand( SysCommand ) ;
	private:
		std::auto_ptr<GGui::Tray> m_tray ;
		std::auto_ptr<Form> m_form ;
		std::auto_ptr<Main::Configuration> m_cfg ;
		bool m_quit ;
		bool m_use_tray ;
		unsigned int m_icon ;
	} ;
	class Menu
	{
		G_EXCEPTION( Error , "menu error" ) ;
		public: explicit Menu( unsigned int resource_id ) ;
		public: ~Menu() ;
		public: int popup( const GGui::WindowBase & w , int sub_pos = 0 ) ;
		private: HMENU m_hmenu ;
		private: HMENU m_hmenu_popup ;
		private: Menu( const Menu & ) ;
		private: void operator=( const Menu & ) ;
	} ;
} ;

// ===

Form::Form( GGui::ApplicationBase & app , Callback & cb , const Main::Configuration & cfg ) :
	m_callback(cb) ,
	m_cfg(cfg) ,
	GGui::Dialog(app) ,
	m_edit_box(*this,IDC_EDIT1)
{
}

bool Form::onInit()
{
	const std::string crlf( "\r\n" ) ;

	std::stringstream ss ;
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

	ss << crlf << Main::CommandLine::warranty(crlf) << crlf << Main::CommandLine::copyright() ;

	m_edit_box.set( ss.str() ) ;

	return true ;
}

void Form::set( const std::string & text )
{
	m_edit_box.set( text ) ;
}

void Form::onClose()
{
	end() ;
}

void Form::close()
{
	end() ;
}

void Form::onNcDestroy()
{
	m_callback.callback() ;
}

// ===

App::App( HINSTANCE h , HINSTANCE p , const char * name ) :
	GGui::ApplicationBase( h , p , name ) ,
	m_use_tray(false) ,
	m_quit(false) ,
	m_icon(0U)
{
}

void App::init( const Main::Configuration & cfg )
{
	m_use_tray = cfg.daemon() ;
	m_cfg <<= new Main::Configuration(cfg) ;
	m_icon = m_cfg->icon() ;
}

void App::callback()
{
	m_form <<= 0 ;
	if( m_use_tray )
		hide() ;
	else
		close() ;
}

void App::onDimension( int & dx , int & dy )
{
	if( m_form.get() )
	{
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
	// (menu and icon resource id, but we have no menus)
	return m_icon == 1U ? IDI_ICON2 : (m_icon == 2U ? IDI_ICON3 : IDI_ICON1) ;
}

bool App::onCreate()
{
	if( m_use_tray )
		m_tray <<= new GGui::Tray(resource(),*this,"E-MailRelay") ;
	else
		doOpen() ;
	return true ;
}

void App::onTrayDoubleClick()
{
	doOpen() ;
}

void App::doOpen()
{
	if( m_form.get() == NULL )
	{
		m_form <<= new Form( *this , *this , *m_cfg.get() ) ;
		if( ! m_form->runModeless(IDD_DIALOG1) )
			throw Error( "cannot run dialog box" ) ;
	}
	resize( externalSize() ) ; // no-op in itself, but uses onDimension()
	show() ;
}

void App::onTrayRightMouseButtonDown()
{
	Menu menu( IDR_MENU1 ) ;
	int id = menu.popup( *this ) ;
	if( id == IDM_OPEN )
		doOpen() ;
	else if( id == IDM_CLOSE )
		doClose() ;
	else
		doQuit() ;
}

void App::doQuit()
{
	m_quit = true ;
	close() ;
}

void App::doClose()
{
	if( m_form.get() )
	{
		m_form->close() ;
		hide() ;
	}
}

bool App::onClose()
{
	if( m_use_tray )
	{
		if( m_quit )
		{
			return true ;
		}
		else
		{
			doClose() ;
			return false ;
		}
	}
	else
	{
		return true ;
	}
}

bool App::onSysCommand( SysCommand sc )
{
	if( sc == scMaximise || sc == scSize )
		return true ; // true <= processed as no-op
	else
		return false ;
}

// ===

Menu::Menu( unsigned int id )
{
	HINSTANCE hinstance = GGui::ApplicationInstance::hinstance() ;
	m_hmenu = ::LoadMenu( hinstance , MAKEINTRESOURCE(id) ) ;
	if( m_hmenu == NULL )
		throw Error() ;
}

int Menu::popup( const GGui::WindowBase & w , int sub_pos )
{
	POINT p ;
	::GetCursorPos( &p ) ;
	::SetForegroundWindow( w.handle() ) ;
	m_hmenu_popup = ::GetSubMenu( m_hmenu , sub_pos ) ;

	const int default_pos = 0 ;
	::SetMenuDefaultItem( m_hmenu_popup , default_pos , TRUE ) ;

	BOOL rc = ::TrackPopupMenuEx( m_hmenu_popup ,
		TPM_RETURNCMD , p.x , p.y , w.handle() , NULL ) ;
	return static_cast<int>(rc) ; // !!
}

Menu::~Menu()
{
	if( m_hmenu != NULL )
		::DestroyMenu( m_hmenu ) ;
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
			Main::Run run( arg ) ;
			G::LogOutput log( run.cfg().log() , run.cfg().verbose() ) ;
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

