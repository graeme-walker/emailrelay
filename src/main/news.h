//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file news.h
///

#ifndef G_MAIN_NEWS_H
#define G_MAIN_NEWS_H

#include "gdef.h"
#include <string>

namespace Main
{
	class News ;
}

//| \class Main::News
/// A static class providing some news text.
///
class Main::News
{
public:
	static std::string text( const std::string & eol ) ;
		///< Returns some 'news' text.

public:
	News() = delete ;
} ;

#endif
