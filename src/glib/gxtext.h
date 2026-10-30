//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gxtext.h
///

#ifndef G_XTEXT_H
#define G_XTEXT_H

#include "gdef.h"
#include "gstringview.h"
#include <string>

namespace G
{
	class Xtext ;
}

//| \class G::Xtext
/// An xtext codec class, encoding space as "+20" etc.
/// \see RFC-1891 section 5
///
class G::Xtext
{
public:
	static std::string encode( std::string_view ) ;
		///< Encodes the given string.

	static std::string decode( std::string_view ) ;
		///< Decodes the given string. Input strings must be
		///< un-strictly valid(), otherwise the result is
		///< undefined.

	static bool valid( std::string_view , bool strict = false ) ;
		///< Returns true if a valid encoding, or empty. If
		///< strict then 'equals' and 'space' are disallowed
		///< and hex characters must be uppercase (eg. "+1A").

public:
	Xtext() = delete ;
} ;

#endif
