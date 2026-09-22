//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file winmenu.h
///

#ifndef G_MAIN_WIN_MENU_H
#define G_MAIN_WIN_MENU_H

#include "gdef.h"
#include "gexception.h"
#include "gwinbase.h"

namespace Main
{
	class WinMenu ;
}

//| \class Main::WinMenu
/// Implements the small pop-up menu for the system tray icon.
///
class Main::WinMenu
{
public:
	G_EXCEPTION( Error , tx("menu error") )

	explicit WinMenu( unsigned int resource_id ) ;
		///< Constructor.

	~WinMenu() ;
		///< Destructor.

	int popup( const GGui::WindowBase & w , bool foreground , bool with_open , bool with_close ) ;
		///< Opens the menu as a popup and returns when the
		///< mouse button is released.
		/// \see TrackPopupMenuEx()

	void update( bool with_open , bool with_close ) ;
		///< Updates the menu, even while popup() is
		///< still running.

public:
	WinMenu( const WinMenu & ) = delete ;
	WinMenu( WinMenu && ) = delete ;
	WinMenu & operator=( const WinMenu & ) = delete ;
	WinMenu & operator=( WinMenu && ) = delete ;

private:
	HMENU m_hmenu {HNULL} ;
	HMENU m_hmenu_popup {HNULL} ;
} ;

#endif

