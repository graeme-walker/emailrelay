//
// Copyright (C) 2001 Graeme Walker <graeme_walker@users.sourceforge.net>
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
// gappinst.h
//

#ifndef G_APPINST_H
#define G_APPINST_H

#include "gdef.h"

namespace GGui
{
	class ApplicationInstance ;
} ;

// Class: GGui::ApplicationInstance
//
// Description: A class for storing the application's
// instance handle, as obtained from WinMain().
//
// Other low-level classes in this library use this
// class to obtain the application instance handle,
// rather than calling GApplication::instance().
//
// Programs which need a message pump, but want to
// avoid the overhead of the full GUI application
// must use this class as an absolute minimum.
// However, they should probably use the GApplicationBase
// class, which also minimises the dependencies on
// the framework.
//
// See also: ApplicationBase
//
class GGui::ApplicationInstance
{
public:
	explicit ApplicationInstance( HINSTANCE hinstance ) ;
		// Constructor. (Setting the static value
		// through a constructor is a bit klunky
		// but it makes it easy to retrofit this
		// class to the original version of
		// GApplication.)

	static HINSTANCE hinstance() ;
		// Returns the instance handle that was
		// passed to the constructor. Returns
		// zero if no GApplicationInstance
		// object has been created.

private:
	static HINSTANCE m_hinstance ;
} ;

#endif
