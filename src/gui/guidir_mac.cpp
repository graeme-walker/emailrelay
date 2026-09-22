//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file guidir_mac.cpp
///

#include "gdef.h"
#include "guidir.h"
#include "gpath.h"
#include "gfile.h"
#include "gdirectory.h"
#include "genvironment.h"
#include "gstrmacros.h"

#ifndef G_CONFDIR
	#define G_CONFDIR
#endif
#ifndef G_SPOOLDIR
	#define G_SPOOLDIR
#endif

namespace Gui
{
	namespace DirImp
	{
		bool ok( const std::string & s ) ;
		std::string rebase( const std::string & dir ) ;
		G::Path envPath( const std::string & key , const G::Path & default_ ) ;
	}
}

G::Path Gui::Dir::install()
{
	// user expects to say "/Applications" or "~/Applications"
	return DirImp::rebase( "/Applications" ) ;
}

G::Path Gui::Dir::config()
{
	std::string confdir( G_STR(G_CONFDIR) ) ;
	if( confdir.empty() )
		confdir = DirImp::rebase( "/Applications/E-MailRelay" ) ;
	return confdir ;
}

G::Path Gui::Dir::spool()
{
	std::string spooldir( G_STR(G_SPOOLDIR) ) ;
	if( spooldir.empty() )
		spooldir = DirImp::rebase( "/Applications/E-MailRelay/Spool" ) ;
	return spooldir ;
}

G::Path Gui::Dir::pid( const G::Path & )
{
	return DirImp::ok("/var/run") ? "/var/run" : "/tmp" ;
}

G::Path Gui::Dir::desktop()
{
	return home() / "Desktop" ;
}

G::Path Gui::Dir::menu()
{
	return G::Path() ;
}

G::Path Gui::Dir::autostart()
{
	return G::Path() ;
}

G::Path Gui::Dir::home()
{
	return DirImp::envPath( "HOME" , "~" ) ;
}

// ==

bool Gui::DirImp::ok( const std::string & s )
{
	return
		!s.empty() &&
		G::File::exists(G::Path(s)) &&
		G::Directory(G::Path(s)).valid() &&
		G::Directory(G::Path(s)).writeable() ;
}

std::string Gui::DirImp::rebase( const std::string & dir )
{
	static bool use_root = DirImp::ok( "/Applications" ) ;
	return (use_root?"":"~") + dir ;
}

G::Path Gui::DirImp::envPath( const std::string & key , const G::Path & default_ )
{
	return G::Environment::getPath( key , default_ ) ;
}

