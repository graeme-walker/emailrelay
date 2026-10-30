//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gexception.cpp
///

#include "gdef.h"
#include "gexception.h"
#include "gstr.h"

G::Exception::Exception( std::initializer_list<std::string_view> args ) :
	std::runtime_error(join(args))
{
}

G::Exception::Exception( std::string_view what ) :
	Exception{what}
{
}

G::Exception::Exception( std::string_view what , std::string_view more ) :
	Exception{what,more}
{
}

G::Exception::Exception( std::string_view what , std::string_view more1 , std::string_view more2 ) :
	Exception{what,more1,more2}
{
}

G::Exception::Exception( std::string_view what , std::string_view more1 , std::string_view more2 ,
	std::string_view more3 ) :
		Exception{what,more1,more2,more3}
{
}

G::Exception::Exception( std::string_view what , std::string_view more1 , std::string_view more2 ,
	std::string_view more3 , std::string_view more4 ) :
		Exception{what,more1,more2,more3,more4}
{
}

std::string G::Exception::join( std::initializer_list<std::string_view> args )
{
	std::string result ;
	for( auto arg : args )
		result.append(": ",result.empty()||arg.empty()?0U:2U).append(arg.data(),arg.size()) ;
	return result ;
}

