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
// gscmap.h
//

#ifndef G_SCMAP_H
#define G_SCMAP_H

#include "gdef.h"

namespace GGui
{
	class SubClassMap ;
} ;

// Class: GGui::SubClassMap
// Description: A class for mapping sub-classed window handles
// to their old window procedures. Note that a sub-class
// map is only required for standard windows such as
// standard controls or standard dialog boxes; when subclassing
// our own windows it is better to store the old window procedure
// function pointer using SetWindowLong().
//
class GGui::SubClassMap
{
public:
	typedef WNDPROC Proc ; // could also be FARPROC -- see CallWindowProc

	SubClassMap() ;
		// Default constructor.

	~SubClassMap() ;
		// Destructor.

	void add( HWND hwnd , Proc proc , void *context = NULL ) ;
		// Adds the given entry to the map.

	Proc find( HWND hwnd , void **context_p = NULL ) ;
		// Finds the entry in the map whith the given
		// window handle. Optionally returns the context
		// pointer by reference.

	void remove( HWND hwnd ) ;
		// Removes the given entry from the map. Typically
		// called when processing a WM_NCDESTROY message.

private:
	SubClassMap( const SubClassMap &other ) ;
	void operator=( const SubClassMap &other ) ;

private:
	enum { SlotsLimit = 80 } ;
	struct Slot
	{
		Proc proc ;
		HWND hwnd ;
		void *context ;
		Slot() : proc(0) , hwnd(0) , context(NULL) {} ;
	} ;
	Slot m_list[SlotsLimit] ;
	unsigned int m_high_water ;
} ;

inline
GGui::SubClassMap::SubClassMap() :
	m_high_water(0U)
{
}

#endif