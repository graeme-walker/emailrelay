//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file guiaccess.h
///

#ifndef GUI_INSTALLER_ACCESS_H
#define GUI_INSTALLER_ACCESS_H

#include "gdef.h"
#include "gpath.h"
#include "gexception.h"
#include "gstringarray.h"
#include <string>

namespace Gui
{
	class Access ;
}

//| \class Gui::Access
/// A static class for modifying file-system permissions.
///
class Gui::Access
{
public:
	static bool modify( const G::Path & , bool ) ;
		///< Modifies the permissions on the given path in
		///< some undefined way. Returns false on error.

public:
	Access() = delete ;
} ;

#endif
