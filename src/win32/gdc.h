//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gdc.h
///

#ifndef G_GUI_DC_H
#define G_GUI_DC_H

#include "gdef.h"

namespace GGui
{
	class DeviceContext ;
	class ScreenDeviceContext ;
}

//| \class GGui::DeviceContext
/// A thin wrapper for a GDI device context corresponding to
/// a window.
/// \see GGui::ScreenDeviceContext
///
class GGui::DeviceContext
{
public:
	explicit DeviceContext( HWND hwnd ) ;
		///< Constructor for a window's device context.
		///<
		///< The GDI device context is released in
		///< the destructor.

	explicit DeviceContext( HDC hdc ) ;
		///< Constructor to wrap the given GDI handle.
		///< The GDI handle typically comes from ::BeginPaint()
		///< while processing a WM_PAINT message.
		///<
		///< The GDI device context is _not_ released in
		///< the destructor.

	~DeviceContext() ;
		///< Destructor.

	HDC handle() const ;
		///< Returns the GDI device context handle.

	HDC extractHandle() ;
		///< Extracts the GDI device context handle.
		///< The destructor will no longer release it.

	HDC operator()() const ;
		///< Returns the GDI device context handle.

	void swapBuffers() ;
		///< If the device context has double buffering
		///< then the two pixel buffers are swapped.
		///< This is typically called after the
		///< "back" buffer has been filled with
		///< a new image.

public:
	DeviceContext( const DeviceContext & ) = delete ;
	DeviceContext( DeviceContext && ) = delete ;
	DeviceContext & operator=( const DeviceContext & ) = delete ;
	DeviceContext & operator=( DeviceContext && ) = delete ;

private:
	HDC m_hdc ;
	HWND m_hwnd ;
	bool m_do_release ;
} ;

//| \class GGui::ScreenDeviceContext
/// A thin wrapper for a GDI device context corresponding to
/// the whole screen.
/// \see GGui::DeviceContext
///
class GGui::ScreenDeviceContext
{
public:
	ScreenDeviceContext() ;
		///< Default constructor.

	~ScreenDeviceContext() ;
		///< Destructor.

	HDC handle() ;
		///< Returns the GDI device context handle.

	HDC operator()() ;
		///< Returns the GDI device context handle.

	int colours() const ;
		///< Returns the number of colours.

	int dx() const ;
		///< Returns the screen width.

	int dy() const ;
		///< Returns the screen height.

	int aspectx() const ;
		///< Returns one part of the screen's aspect ratio.

	int aspecty() const ;
		///< Returns the other part of the screen's aspect ratio.

public:
	ScreenDeviceContext( const ScreenDeviceContext & ) = delete ;
	ScreenDeviceContext( ScreenDeviceContext && ) = delete ;
	ScreenDeviceContext & operator=( const ScreenDeviceContext & ) = delete ;
	ScreenDeviceContext & operator=( ScreenDeviceContext && ) = delete ;

private:
	HDC m_dc ;
} ;

#endif
