//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file guiboot.h
///

#ifndef GUI_BOOT_H
#define GUI_BOOT_H

#include "gdef.h"
#include "gpath.h"
#include "gstringarray.h"

namespace Gui
{
	class Boot ;
}

//| \class Gui::Boot
/// Provides support for installing, uninstalling and starting
/// a boot-time service.
///
/// The Windows implementation uses the interface in
/// "servicecontrol.h": service_install(), service_remove()
/// and service_start().
///
/// The Unix implementation uses a start/stop script in
/// /etc/init.d or /etc/rc.d, "update-rc.d" or "rc-update"
/// and "service start". This works for SysV, BSD, systemd
/// and OpenRC because of their various cross-compatibility
/// features.
///
class Gui::Boot
{
public:
	static bool installable() ;
		///< Returns true if the operating-system is supported and the
		///< boot-system directory is valid and accessible.

	static void install( const std::string & name ,
		const G::Path & path_1 , const G::Path & path_2 ) ;
			///< Installs the target as a boot-time service. Throws on error.
			///<
			///< For Windows path_1 is the the batch file and
			///< path_2 is the service wrapper.
			///<
			///< For Unix path_1 is the startstop script and
			///< path_2 is the server executable.

	static bool uninstall( const std::string & name ,
		const G::Path & path_1 , const G::Path & path_2 ) ;
			///< Uninstalls the target as a boot-time service. Returns
			///< false on error or nothing-to-do.

	static bool installed( const std::string & name ) ;
		///< Returns true if currently installed.

	static bool launchable( const std::string & name ) ;
		///< Returns true if launch() is possible.

	static void launch( const std::string & name ) ;
		///< Starts the service.

public:
	Boot() = delete ;
} ;

#endif
