//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gscmap.h
///

#ifndef G_GUI_SCMAP_H
#define G_GUI_SCMAP_H

#include "gdef.h"
#include <vector>

namespace GGui
{
	class SubClassMap ;
}

//| \class GGui::SubClassMap
/// A class for mapping sub-classed window handles to their old
/// window procedures. Note that a sub-class map is only required
/// for standard windows such as standard controls or standard
/// dialog boxes; when subclassing our own windows it is better
/// to store the old window procedure function pointer using
/// SetWindowLong().
///
class GGui::SubClassMap
{
public:
	using Proc = WNDPROC ; // see CallWindowProc

	SubClassMap() ;
		///< Constructor.

	void add( HWND hwnd , Proc proc , void * context = nullptr ) ;
		///< Adds the given entry to the map.

	Proc find( HWND hwnd , void ** context_p = nullptr ) ;
		///< Finds the entry in the map whith the given
		///< window handle. Optionally returns the context
		///< pointer by reference.

	void remove( HWND hwnd ) ;
		///< Removes the given entry from the map. Typically
		///< called when processing a WM_NCDESTROY message.

public:
	SubClassMap( const SubClassMap & ) = delete ;
	SubClassMap( SubClassMap && ) = delete ;
	SubClassMap & operator=( const SubClassMap & ) = delete ;
	SubClassMap & operator=( SubClassMap && ) = delete ;

private:
	struct Slot
	{
		Proc proc {nullptr} ;
		HWND hwnd {HNULL} ;
		void * context {nullptr} ;
		Slot() = default ;
		Slot( Proc proc_in , HWND hwnd_in , void * context_in ) : proc(proc_in) , hwnd(hwnd_in) , context(context_in) {}
	} ;
	std::vector<Slot> m_list ;
} ;

#endif
