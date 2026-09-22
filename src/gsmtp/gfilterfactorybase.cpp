//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gfilterfactorybase.cpp
///

#include "gdef.h"
#include "gfilterfactorybase.h"

GSmtp::FilterFactoryBase::Spec::Spec()
= default ;

GSmtp::FilterFactoryBase::Spec::Spec( std::string_view first_ , std::string_view second_ ) :
	first(G::sv_to_string(first_)) ,
	second(G::sv_to_string(second_))
{
}

GSmtp::FilterFactoryBase::Spec & GSmtp::FilterFactoryBase::Spec::operator+=( const Spec & rhs )
{
	if( first.empty() && !second.empty() )
	{
		; // already in error state
	}
	else if( rhs.first.empty() )
	{
		first.clear() ; // error state
		second = rhs.second ;
	}
	else
	{
		second.append(",",second.empty()?0U:1U).append(rhs.first).append(1U,':').append(rhs.second) ;
	}
	return *this ;
}

