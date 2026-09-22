//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file guilegal.h
///

#ifndef GUI_LEGAL_H
#define GUI_LEGAL_H

#include "gdef.h"
#include <string>
#include <vector>

namespace Gui
{
	class Legal ;
}

//| \class Gui::Legal
/// A static class providing warranty and copyright text.
///
class Gui::Legal
{
public:
	static const char * text() ;
		///< Returns the introductory legal text.

	static const char * license() ;
		///< Returns the license text.

	static std::vector<std::string> credits() ;
		///< Returns the third-party library credits.

public:
	Legal() = delete ;
} ;

#endif
