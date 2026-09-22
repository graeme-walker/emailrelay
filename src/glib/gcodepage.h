//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gcodepage.h
///

#ifndef G_CODEPAGE_H
#define G_CODEPAGE_H

#include "gdef.h"
#include "gstringview.h"

namespace G
{
	namespace CodePage /// Windows codepage conversion functions.
	{
		std::string fromCodePage850( std::string_view s ) ;
			///< Converts from codepage 850 to UTF-8.

		std::string toCodePage850( std::string_view s ) ;
			///< Converts from UTF-8 to codepage 850.

		std::string fromCodePage1252( std::string_view s ) ;
			///< Converts from codepage 1252 to UTF-8.

		std::string toCodePage1252( std::string_view s ) ;
			///< Converts from UTF-8 to codepage 1252.

		std::string toCodePageOem( std::string_view ) ;
			///< Converts from UTF-8 to the active OEM codepage
			///< (see GetOEMCP(), 850 on unix).

		std::string fromCodePageOem( std::string_view ) ;
			///< Converts from the active OEM codepage
			///< (see GetOEMCP(), 850 on unix) to UTF-8.

		std::string toCodePageAnsi( std::string_view ) ;
			///< Converts from UTF-8 to the active "ansi" codepage
			///< (see GetACP(), 1252 on unix).

		std::string fromCodePageAnsi( std::string_view ) ;
			///< Converts from the active OEM codepage
			///< (see GetACP(), 1252 on unix) to UTF-8.

		constexpr char oem_error = '\xDB' ; // OEM error character -- full block
		constexpr char ansi_error = '\xBF' ; // Ansi error character -- inverted question mark
	}
}

#endif
