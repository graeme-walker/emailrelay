//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gappinst.h
///

#ifndef G_GUI_APPINST_H
#define G_GUI_APPINST_H

#include "gdef.h"

namespace GGui
{
	class ApplicationInstance ;
}

//| \class GGui::ApplicationInstance
/// A class for storing the application's instance handle, as
/// obtained from WinMain().
///
/// Other low-level classes in this library use this interface to
/// obtain the application instance handle, rather than some
/// higher-level mechanism.
///
/// Programs that need a message pump, but want to avoid the
/// overhead of the full GUI application framework must, as an
/// absolute minimum, use this class to set the application
/// instance handle.
///
/// \see GGui::ApplicationBase
///
class GGui::ApplicationInstance
{
protected:
	explicit ApplicationInstance( HINSTANCE h ) ;
		///< Protected constructor that calls
		///< hinstance(h).

public:
	static void hinstance( HINSTANCE h ) ;
		///< Sets the instance handle, which is
		///< subsequently returned by hinstance().

	static HINSTANCE hinstance() ;
		///< Returns the instance handle that was
		///< passed to the constructor. Returns
		///< zero if hinstance(h) has never been
		///< called.

private:
	static HINSTANCE m_hinstance ;
} ;

#endif
