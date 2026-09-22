//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file guidir.h
///

#ifndef G_MAIN_GUI_DIR_H
#define G_MAIN_GUI_DIR_H

#include "gdef.h"
#include "gpath.h"
#include <string>
#include <iostream>

namespace Gui
{
	class Dir ;
}

//| \class Gui::Dir
/// Provides default file-system paths. The implementations
/// of this interface are highly platform-specific.
///
class Gui::Dir
{
public:
	static G::Path install() ;
		///< Returns the default install directory, eg. "/usr".

	static G::Path config() ;
		///< Returns the configuration directory path, eg. "/etc".

	static G::Path spool() ;
		///< Returns the spool directory path, eg. "/var/spool".

	static G::Path home() ;
		///< Returns the user's home directory, eg. "/home/username".

	static G::Path pid( const G::Path & config_dir ) ;
		///< Returns the directory for pid files, eg. "/run".

	static G::Path desktop() ;
		///< Returns the desktop path, eg. "/home/username/Desktop".

	static G::Path autostart() ;
		///< Returns the autostart directory path, eg. "/home/username/AutoStart".

	static G::Path menu() ;
		///< Returns the menu path, eg. "/home/username/.local/share/applications".

public:
	Dir() = delete ;
} ;

#endif
