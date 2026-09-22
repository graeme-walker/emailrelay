//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file guiboot_unix.cpp
///

#include "gdef.h"
#include "guiboot.h"
#include "gpath.h"
#include "gstr.h"
#include "gfile.h"
#include "gexecutablecommand.h"
#include "gnewprocess.h"
#include "glog.h"
#include <sstream>
#include <stdexcept>

namespace Gui
{
	namespace BootImp
	{
		int run( std::string exe , G::StringArray args , std::string & output )
		{
			using Fd = G::NewProcess::Fd ;
			G::NewProcess task( exe , args ,
				G::NewProcess::Config()
					.set_stdout( Fd::devnull() )
					.set_stderr( Fd::pipe() )
					.set_env( G::Environment::minimal(true) ) // (no HOME)
					.set_exec_error_format( "failed to execute ["+exe+"]: __""strerror""__" )
					.set_exec_search_path( "/usr/bin:/usr/sbin:/bin:/sbin" )
					.set_strict_exe( false ) ) ;
			int rc = task.waitable().wait().get() ;
			output = G::Str::printable( G::Str::trimmed(task.waitable().output(),G::Str::ws()) ) ;
			G_LOG( "Gui::BootImp::run: exe=[" << exe << "] args=[" << G::Str::join(",",args) << "] "
				<< "rc=" << rc << " output=[" << output << "]" ) ;
			return rc ;
		}
		int run( std::string exe , G::StringArray args )
		{
			std::string output ;
			return run( exe , args , output ) ;
		}
		G::Path dir_boot( bool alt = false )
		{
			if( G::is_bsd() )
				return alt ? "/usr/local/etc/rc.d" : "/etc/rc.d" ;
			else
				return "/etc/init.d" ;
		}
	}
}

bool Gui::Boot::installable()
{
	using namespace BootImp ;
	return G::Identity::real().isRoot() && G::File::isDirectory( dir_boot() , std::nothrow ) ;
}

void Gui::Boot::install( const std::string & name , const G::Path & startstop_src , const G::Path & )
{
	using namespace BootImp ;
	if( startstop_src != (dir_boot()/name) )
		G::File::copy( startstop_src , dir_boot()/name ) ;
	G::File::chmodx( dir_boot()/name , std::nothrow ) ;
	bool ok = run( "update-rc.d" , {name,"defaults"} ) == 0 || run( "rc-update" , {"add",name} ) == 0 ;
	if( !ok )
		throw std::runtime_error( "failed to run update-rc" ) ;
}

bool Gui::Boot::uninstall( const std::string & name , const G::Path & , const G::Path & )
{
	using namespace BootImp ;
	G::File::remove( dir_boot()/name , std::nothrow ) ;
	bool ok = run( "update-rc.d" , {"-f",name,"remove"} ) == 0 || run( "rc-update" , {"-a","delete",name} ) == 0 ;
	return ok ;
}

bool Gui::Boot::installed( const std::string & name )
{
	using namespace BootImp ;
	return
		G::File::exists( dir_boot()/name , std::nothrow ) ||
		G::File::exists( dir_boot(true)/name , std::nothrow ) ;
}

bool Gui::Boot::launchable( const std::string & name )
{
	return installed( name ) ;
}

void Gui::Boot::launch( const std::string & name )
{
	using namespace BootImp ;
	std::string output ;
	bool ok = run( "service" , {name,"start"} , output ) == 0 ;
	if( !ok && output.empty() )
		output = "error" ;
	if( !ok )
		throw std::runtime_error( "failed to run [service "+name+" start]: " + output ) ;
}

