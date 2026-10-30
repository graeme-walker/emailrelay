//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file ggettext_unix.cpp
///

#include "gdef.h"
#include "ggettext.h"
#include <clocale>
#include <libintl.h>

void G::gettext_init( const std::string & localedir , const std::string & appname )
{
	if( !appname.empty() )
	{
		std::setlocale( LC_MESSAGES , "" ) ;
		std::setlocale( LC_CTYPE , "" ) ;
		if( !localedir.empty() )
            bindtextdomain( appname.c_str() , localedir.c_str() ) ;
		textdomain( appname.c_str() ) ;
		// see also bind_textdomain_codeset()
	}
}

const char * G::gettext( const char * p ) noexcept
{
	return ::gettext( p ) ;
}

