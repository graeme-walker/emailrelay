//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file guiboot_mac.cpp
///

#include "gdef.h"
#include "guiboot.h"
#include "gpath.h"
#include "gfile.h"
#include "gdirectory.h"
#include <stdexcept>

namespace Gui
{
	namespace BootImp
	{
		G::Path dir_boot()
		{
			return "/Library/StartupItems" ;
		}
	}
}

bool Gui::Boot::installable()
{
	using namespace BootImp ;
	G::File::mkdirs( dir_boot() , std::nothrow , 6 ) ; // hmm
	return
		G::Directory(dir_boot()).valid(true) &&
		G::Directory(dir_boot()).writeable() ; // (creates a probe file)
}

void Gui::Boot::install( const std::string & , const G::Path & , const G::Path & exe )
{
	using namespace BootImp ;
	G::Path plist_src = exe.dirname() + "StartupParameters.plist" ;
	G::File::mkdirs( dir_boot() + exe.basename() , std::nothrow , 6 ) ;
	bool ok =
		G::File::copy( exe , dir_boot() + exe.basename() + exe.basename() , std::nothrow ) &&
		G::File::copy( plist_src , dir_boot() + exe.basename() + plist_src.basename() , std::nothrow ) ;
	if( !ok )
		throw std::runtime_error( "failed to install startup items" ) ;
}

bool Gui::Boot::uninstall( const std::string & , const G::Path & , const G::Path & exe )
{
	using namespace BootImp ;
	return
		G::File::remove( dir_boot() + exe.basename() + exe.basename() , std::nothrow ) &&
		G::File::remove( dir_boot() + exe.basename() + "StartupParameters.plist" , std::nothrow ) &&
		G::File::remove( dir_boot() + exe.basename() , std::nothrow ) ;
}

bool Gui::Boot::installed( const std::string & name )
{
	using namespace BootImp ;
	return G::File::exists( dir_boot() + name + name , std::nothrow ) ; // not tested
}

bool Gui::Boot::launchable( const std::string & )
{
	return false ;
}

void Gui::Boot::launch( const std::string & )
{
	// TODO Gui::Boot::launch() for mac
	throw std::runtime_error( "startup failed: not implemented" ) ;
}

