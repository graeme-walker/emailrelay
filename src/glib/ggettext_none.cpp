//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file ggettext_none.cpp
///

#include "gdef.h"
#include "ggettext.h"

void G::gettext_init( const std::string & , const std::string & )
{
}

const char * G::gettext( const char * p ) noexcept
{
	return p ;
}

