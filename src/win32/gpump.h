//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gpump.h
///

#ifndef G_GUI_PUMP_H
#define G_GUI_PUMP_H

#include "gdef.h"
#include <string>
#include <utility>

namespace GGui
{
	class Pump ;
}

//| \class GGui::Pump
/// A static class which implements a Windows GetMessage/DispatchMessage
/// message pump. While the pump is run()ning it pulls messages out
/// of the message queue and dispatches them to the relevant window
/// procedure.
///
/// Uses GGui::Dialog::dialogMessage() in its implementation in order
/// to support modeless dialog boxes.
///
/// The implementation guarantees that there will be no extraneous
/// calls to PeekMessage() that might upset MsgWaitForMultipleObjects().
///
/// \see GGui::Cracker, GGui::Dialog, GGui::ApplicationInstance
///
class GGui::Pump
{
public:
	static std::string run() ;
		///< Runs the GetMessage()/DispatchMessage() message pump.
		///< Returns a reason string if quit() was called.

	static std::pair<bool,std::string> runToEmpty() ;
		///< Runs the PeekMessage()/DispatchMessage() message pump
		///< until the message queue gets to empty. Returns true and
		///< a reason string if quit() was called at some point.

	static void quit( const std::string & reason = {} ) ;
		///< Causes run() to return as soon as the message queue
		///< is empty and the call stack has unwound. Also
		///< sets the return value for runToEmpty().

public:
	Pump() = delete ;

private:
	static bool getMessage( MSG * , bool ) ;
	static bool empty() ;
	static std::pair<bool,std::string> runImp( bool ) ;
	static WPARAM m_run_id ;
	static std::string m_quit_reason ;
} ;

#endif
