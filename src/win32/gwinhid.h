//
// Copyright (C) 2001-2002 Graeme Walker <graeme_walker@users.sourceforge.net>
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
// gwinhid.h
//	

#ifndef G_WINHID_H
#define G_WINHID_H

#include "gdef.h"
#include "gwindow.h"
#include <string>

namespace GGui
{
	class WindowHidden ;
} ;

// Class: GGui::WindowHidden
// Description: A derivation of GWindow for
// a hidden window (without a parent).
//
class GGui::WindowHidden : public Window
{
public:
	explicit WindowHidden( HINSTANCE hinstance ) ;
		// Default constructor. Registers the window
		// class if necessary and creates the window.

	virtual ~WindowHidden() ;
		// Virtual destructor.


private:
	virtual void onNcDestroy() ;
	WindowHidden( const WindowHidden & ) ;
	void operator=( const WindowHidden & ) ;
	std::string windowClassName() ;

private:
	bool m_destroyed ;
	static bool m_registered ;
} ;

#endif
