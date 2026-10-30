//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file genvironment_win32.cpp
///

#include "gdef.h"
#include "gnowide.h"
#include "genvironment.h"

std::string G::Environment::get( const std::string & name , const std::string & default_ )
{
	return nowide::getenv( name , default_ ) ;
}

G::Path G::Environment::getPath( const std::string & name , const G::Path & default_ )
{
	return { nowide::getenv( name , default_.str() ) } ;
}

G::Environment G::Environment::minimal( bool )
{
	return Environment( {} ) ;
}

void G::Environment::put( const std::string & name , const std::string & value )
{
	nowide::putenv( name , value ) ;
}

