//
// Copyright (C) 2001-2003 Graeme Walker <graeme_walker@users.sourceforge.net>
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
// winapp.h
//

#ifndef WIN_APP_H
#define WIN_APP_H

#include "gdef.h"
#include "gappbase.h"
#include "gexception.h"
#include "gtray.h"
#include "winform.h"
#include "configuration.h"
#include <memory>

namespace Main
{
	class WinApp ;
}

class Main::WinApp : public GGui::ApplicationBase
{
public:
	G_EXCEPTION( Error , "application error" ) ;
	WinApp( HINSTANCE h , HINSTANCE p , const char * name ) ;
	void init( const Main::Configuration & cfg ) ;
	bool confirm() ;
	void formOk() ;
	void formDone() ;
	void onRunEvent( std::string , std::string , std::string ) ;

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
	void setStatus( const std::string & , const std::string & ) ;

private:
	std::auto_ptr<GGui::Tray> m_tray ;
	std::auto_ptr<Main::WinForm> m_form ;
	std::auto_ptr<Main::Configuration> m_cfg ;
	bool m_quit ;
	bool m_use_tray ;
	unsigned int m_icon ;
	bool m_hidden ;
} ;

#endif

