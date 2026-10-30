//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gbase64.h
///

#ifndef G_BASE64_H
#define G_BASE64_H

#include "gdef.h"
#include "gexception.h"
#include "gstringview.h"
#include <string>

namespace G
{
	class Base64 ;
}

//| \class G::Base64
/// A base64 codec class.
/// \see RFC-1341 section 5.2
///
class G::Base64
{
public:
	G_EXCEPTION( Error , tx("base64 encoding error") )

	static std::string encode( std::string_view , std::string_view line_break = {} ) ;
		///< Encodes the given string, optionally inserting line-breaks
		///< to limit the line length.

	static std::string decode( std::string_view , bool throw_on_invalid = false , bool strict = true ) ;
		///< Decodes the given string. Either throws an exception if
		///< not a valid() encoding, or returns the empty string.

	static bool valid( std::string_view , bool strict = true ) ;
		///< Returns true if the string is a valid base64 encoding,
		///< possibly allowing for embedded newlines, carriage returns,
		///< and space characters. Strict checking permits one or
		///< two pad characters at the end to make a multiple of
		///< four characters in total, but no newlines, carriage
		///< returns or other odd characters. Empty strings
		///< are valid; single character strings are not.

public:
	Base64() = delete ;
} ;

#endif
