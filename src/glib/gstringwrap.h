//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gstringwrap.h
///

#ifndef G_STRING_WRAP_H
#define G_STRING_WRAP_H

#include "gdef.h"
#include "gstringview.h"
#include <string>

namespace G
{
	class StringWrap ;
}

//| \class G::StringWrap
/// A word-wrap class.
///
class G::StringWrap
{
public:
	static std::string wrap( const std::string & text ,
		const std::string & prefix_first , const std::string & prefix_other ,
		std::size_t width_first = 70U , std::size_t width_other = 0U ,
		bool preserve_spaces = false ) ;
			///< Does word-wrapping of UTF-8 text. The return value is a string
			///< with embedded newlines. If 'preserve_spaces' is true then all
			///< space characters between input words that end up in the middle
			///< of an output line are preserved. There is no special handling
			///< of tabs or carriage returns. The 'first/other' parameters
			///< distinguish between the first output line and the rest.

	static std::size_t wordsize( const std::string & ) ;
		///< Returns the number of characters in UTF-8 text.

public:
	StringWrap() = delete ;
} ;

#endif
