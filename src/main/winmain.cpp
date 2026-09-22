//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file winmain.cpp
///

#include "gdef.h"
#include "garg.h"
#include "gslot.h"
#include "gexception.h"
#include "winapp.h"
#include "commandline.h"
#include "options.h"
#include "run.h"
#include "resource.h"
#include "gcontrol.h"
#include <clocale>

int WINAPI WinMain( HINSTANCE hinstance , HINSTANCE previous , LPSTR /*command_line*/ , int show_style )
{
	try
	{
		GGui::Control::init() ; // early DPI awareness

		G::Arg arg = G::Arg::windows() ; // GetCommandLineW()

		Main::WinApp app( hinstance , previous , "E-MailRelay" ) ;
		Main::Run run( app , arg , /*has-gui=*/true ) ;
		try
		{
			G::Options options_spec = Main::Options::spec() ;
			run.configure( options_spec ) ;
			if( run.hidden() )
				app.disableOutput() ;

			if( run.runnable() )
			{
				app.init( run.configuration(0U) , options_spec ) ;
				app.createWindow( show_style , /*do_show=*/false , 10 , 10 ) ; // main window, not shown
				run.signal().connect( G::Slot::slot(app,&Main::WinApp::onRunEvent) ) ;
				run.run() ;
			}
		}
		catch( GNet::SocketBase::SocketBindError & e )
		{
			if( e.m_einuse )
			{
				std::string_view help = "check whether emailrelay is already running as a service" ;
				app.onError( std::string(e.what()).append(": ",2U).append(help.data(),help.size()) , 2 ) ;
			}
			else
			{
				app.onError( e.what() , 2 ) ;
			}
		}
		catch( std::exception & e )
		{
			app.onError( e.what() , 1 ) ;
		}

		return app.exitCode() ;
	}
	catch(...)
	{
		MessageBeep( MB_ICONHAND ) ;
	}
	return 1 ;
}

