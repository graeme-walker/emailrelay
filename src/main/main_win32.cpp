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

#include "gdef.h"
#include "gsmtp.h"
#include "run.h"
#include "configuration.h"
#include "commandline.h"
#include "legal.h"
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
#include "gassert.h"

namespace
{
	class Callback
	{
		public: virtual void callback() = 0 ;
	} ;

	class Form : public GGui::Dialog
	{
	public:
		Form( GGui::ApplicationBase & , Callback & ,
			const Main::Configuration & cfg , bool confirm ) ;
		void close() ;
	private:
		virtual bool onInit() ;
		virtual void onNcDestroy() ;
		virtual void onClose() ;
		virtual void onCommand( unsigned int id ) ;
		std::string text() const ;
	private:
		GGui::ApplicationBase & m_app ;
		Callback & m_callback ;
		GGui::EditBox m_edit_box ;
		Main::Configuration m_cfg ;
		bool m_confirm ;
	} ;

	class App : public GGui::ApplicationBase , private Callback
	{
	public:
		G_EXCEPTION( Error , "application error" ) ;
		App( HINSTANCE h , HINSTANCE p , const char * name ) ;
		void init( const Main::Configuration & cfg ) ;
		void setStatus( const std::string & , const std::string & ) ;
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
	public:
		G_EXCEPTION( Error , "menu error" ) ;
		explicit Menu( unsigned int resource_id ) ;
		~Menu() ;
		int popup( const GGui::WindowBase & w , int sub_pos = 0 ) ;
	private:
		HMENU m_hmenu ;
		HMENU m_hmenu_popup ;
		Menu( const Menu & ) ;
		void operator=( const Menu & ) ;
	} ;

	class Run : public Main::Run
	{
		public: Run( App & app , const G::Arg & ) ;
		protected: void onStatusChange( const std::string & , const std::string & ) ;
		private: App & m_app ;
	} ;
} ;

// ===

Form::Form( GGui::ApplicationBase & app , Callback & cb , const Main::Configuration & cfg , bool confirm ) :
	GGui::Dialog(app) ,
	m_app(app) ,
	m_callback(cb) ,
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

	ss
		<< crlf << Main::Legal::warranty(crlf)
		<< crlf << Main::Legal::copyright() ;

	return ss.str() ;
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

void Form::onCommand( unsigned int id )
{
	if( id == IDOK ) // always true?
	{
		bool really = true ;
		if( m_confirm )
			really = m_app.messageBoxQuery( "Really quit?" ) ;
		if( really )
			end() ;
	}
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
	m_icon = m_cfg->icon() % 4U ;
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

void App::onTrayDoubleClick()
{
	doOpen() ;
}

void App::doOpen()
{
	if( m_form.get() == NULL )
	{
		m_form <<= new Form( *this , *this , *m_cfg.get() , !m_use_tray ) ;
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
		if( m_quit ) // if called as a result of doQuit()
		{
			return true ;
		}
		else
		{
			doClose() ;
			return false ; // false <= keep running
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
		return true ; // true <= processed as no-op => dont change
	else
		return false ;
}

void App::setStatus( const std::string & s1 , const std::string & s2 )
{
	// simple implementation for now...

	std::string s0( title() ) ;
	std::string message( s0 ) ;
	if( !s1.empty() )
	{
		message.append( ": " ) ;
		message.append( s1 ) ;
	}
	if( !s2.empty() )
	{
		message.append( ": " ) ;
		message.append( s2 ) ;
	}
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

int Menu::popup( const GGui::WindowBase & w , int sub_pos )
{
	POINT p ;
	::GetCursorPos( &p ) ;
	::SetForegroundWindow( w.handle() ) ;

	// TrackPopup() only works with a sub-menu, although
	// you would never guess from the documentation
	//
	m_hmenu_popup = ::GetSubMenu( m_hmenu , sub_pos ) ;

	// make the "open" menu item bold
	//
	const int default_pos = 0 ;
	::SetMenuDefaultItem( m_hmenu_popup , default_pos , TRUE ) ;

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
	m_app(app)
{
}

void Run::onStatusChange( const std::string & s1 , const std::string & s2 )
{
	m_app.setStatus( s1 , s2 ) ;
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

