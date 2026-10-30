//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file ggettext.h
///

#ifndef G_GETTEXT_H
#define G_GETTEXT_H

#include "gdef.h"
#include "gstringview.h"
#include <string>

// String literals should be marked for translation using gettext() or
// gettext_noop(), but not using the "G::" namespace scoping so that
// 'xgettext(1)' will still work. For brevity G::txt() or G::tx()
// can be used instead. See also G::format.
//
// Eg:
/// \code
/// #include "ggettext.h"
/// using G::tx ;
/// using G::txt ;
/// Message msg( tx("world") ) ; // Message ctor calls gettext()
/// std::cout << txt("hello") << msg.translated() << "\n" ;
/// \endcode

namespace G
{
	void gettext_init( const std::string & localedir , const std::string & name ) ;
		///< Initialises the gettext() library. This uses environment variables
		///< to set the CTYPE and MESSAGES facets of the global C locale as a
		///< side-effect.

	const char * gettext( const char * ) noexcept ;
		///< Returns the message translation in the current locale's codeset,
		///< eg. ISO-8859-1 or UTF-8, transcoding from the catalogue as
		///< necessary.

	constexpr const char * gettext_noop( const char * p ) noexcept ;
		///< Returns the parameter. Used to mark a string-literal for
		///< translation, with the conversion at run-time done with
		///< a call to gettext() elsewhere in the code.
		///<
		///< \code
		///< using G::gettext_noop ;
		///< std::cout << call_gettext( gettext_noop("hello, world") ) ;
		///< \endcode

	const char * txt( const char * p ) noexcept ;
		///< A briefer alternative to G::gettext().

	constexpr const char * tx( const char * p ) noexcept ;
		///< A briefer alternative to G::gettext_noop().

	constexpr std::string_view tx( std::string_view sv ) noexcept ;
		///< String view overload.
}

inline const char * G::txt( const char * p ) noexcept
{
	return G::gettext( p ) ;
}

constexpr const char * G::gettext_noop( const char * p ) noexcept
{
	return p ;
}

constexpr const char * G::tx( const char * p ) noexcept
{
	return p ;
}

constexpr std::string_view G::tx( std::string_view sv ) noexcept
{
	return sv ;
}

#endif
