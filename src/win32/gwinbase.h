//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gwinbase.h
///

#ifndef G_GUI_WINBASE_H
#define G_GUI_WINBASE_H

#include "gdef.h"
#include "gsize.h"

namespace GGui
{
	class WindowBase ;
}

//| \class GGui::WindowBase
/// A low-level window class that encapsulates a window handle
/// and provides methods to retrieve basic window attributes.
/// Knows nothing about window messages.
/// \see GGui::Cracker, GGui::Window, GGui::Dialog
///
class GGui::WindowBase
{
public:
	explicit WindowBase( HWND hwnd ) ;
		///< Constructor.

	virtual ~WindowBase() ;
		///< Virtual destructor.

	HWND handle() const noexcept ;
		///< Returns the window handle.

	Size externalSize() const ;
		///< Returns the external size of the window.

	Size internalSize() const ;
		///< Returns the internal size of the window.
		///< (ie. the size of the client area)

	std::string windowClass() const ;
		///< Returns the window's window-class name.

	HINSTANCE windowInstanceHandle() const ;
		///< Returns the window's application instance.
		/// \see GGui::ApplicationInstance

protected:
	void setHandle( HWND hwnd ) noexcept ;
		///< Sets the window handle.

public:
	WindowBase( const WindowBase & ) = delete ;
	WindowBase( WindowBase && ) = delete ;
	WindowBase & operator=( const WindowBase & ) = delete ;
	WindowBase & operator=( WindowBase && ) = delete ;

private:
	HWND m_hwnd ;
} ;

inline
HWND GGui::WindowBase::handle() const noexcept
{
	return m_hwnd ;
}

#endif
