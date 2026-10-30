//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file legal.h
///

#ifndef G_MAIN_LEGAL_H
#define G_MAIN_LEGAL_H

#include "gdef.h"
#include <string>

namespace Main
{
	class Legal ;
}

//| \class Main::Legal
/// A static class providing warranty and copyright text.
///
class Main::Legal
{
public:
	static std::string warranty( const std::string & prefix , const std::string & eol ) ;
		///< Returns the warranty text.

	static std::string copyright() ;
		///< Returns the copyright text.

public:
	Legal() = delete ;
} ;

#endif
