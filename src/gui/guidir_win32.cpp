//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file guidir_win32.cpp
///

#include "gdef.h"
#include "gnowide.h"
#include "gconvert.h"
#include "guidir.h"
#include "gfile.h"
#include "gpath.h"
#include "genvironment.h"
#include "glog.h"
#include <stdexcept>
#include <vector>
#include <shlwapi.h>
#include <shlobj.h>

#ifndef SHGFP_TYPE_CURRENT
#define SHGFP_TYPE_CURRENT 0
#endif
#ifndef CSIDL_PROGRAM_FILES
#define CSIDL_PROGRAM_FILES 38
#endif
#ifndef CSIDL_PROGRAM_FILESX86
#define CSIDL_PROGRAM_FILESX86 42
#endif

namespace Gui
{
	namespace DirImp
	{
		int special_id( const std::string & type ) ;
		G::Path special( const std::string & type ) ;
		G::Path envPath( const std::string & key , const G::Path & default_ ) ;
		G::Path home() ;
	}
}

G::Path Gui::Dir::install()
{
	return DirImp::special("programs") / "E-MailRelay" ;
}

G::Path Gui::Dir::config()
{
	return DirImp::special("data") / "E-MailRelay" ;
}

G::Path Gui::Dir::spool()
{
	return DirImp::special("data") / "E-MailRelay" / "spool" ;
}

G::Path Gui::Dir::pid( const G::Path & )
{
	return DirImp::special("data") / "E-MailRelay" ;
}

G::Path Gui::Dir::home()
{
	return DirImp::envPath( "USERPROFILE" , DirImp::envPath( "HOME" , desktop() ) ) ;
}

G::Path Gui::Dir::desktop()
{
	return DirImp::special( "desktop" ) ;
}

G::Path Gui::Dir::autostart()
{
	return DirImp::special( "autostart" ) ;
}

G::Path Gui::Dir::menu()
{
	return DirImp::special( "menu" ) ;
}

// ==

G::Path Gui::DirImp::special( const std::string & type )
{
	// this is not quite right when running with UAC administrator rights because
	// it gets the administrator's user directories for the desktop etc links and not
	// the user's -- and there is no reasonable way to get the user's access token
	HANDLE user_token = HNULL ; // TODO original user's paths when run-as administrator
	G::Path result = G::nowide::shGetFolderPath( HNULL , special_id(type) , user_token , SHGFP_TYPE_CURRENT ) ;
	return result.empty() ? G::Path("c:/") : result ;
}

int Gui::DirImp::special_id( const std::string & type )
{
	if( type == "desktop" )
		return CSIDL_DESKTOPDIRECTORY ; // "c:/users/<username>/desktop"
	else if( type == "menu" )
		return CSIDL_PROGRAMS ; // "c:/users/<username>/appdata/roaming/microsoft/windows/start menu/programs"
	else if( type == "autostart" )
		return CSIDL_STARTUP ; // "c:/users/<username>/appdata/roaming/microsoft/windows/start menu/startup/programs"
	else if( type == "programs" && sizeof(void*) == 4 )
		return CSIDL_PROGRAM_FILESX86 ; // "c:/program files (x86)"
	else if( type == "programs" )
		return CSIDL_PROGRAM_FILES ; // "c:/program files"
	else if( type == "data" )
		return CSIDL_COMMON_APPDATA ; // "c:/programdata"
	else
		throw std::runtime_error("internal error") ;
}

G::Path Gui::DirImp::envPath( const std::string & key , const G::Path & default_ )
{
	return G::Environment::getPath( key , default_ ) ;
}

