//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
// 
// Copyright (c) 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// ===
///
/// \file goption.cpp
///

#include "gdef.h"
#include "goption.h"
#include "gstr.h"

G::Option::Option( char c , std::string_view name , std::string_view description ,
	std::string_view description_extra , Multiplicity value_multiplicity ,
	std::string_view value_description , unsigned int level ,
	unsigned int main_tag , unsigned int tag_bits ) :
		m_c(c) ,
		m_name(sv_to_string(name)) ,
		m_description(sv_to_string(description)) ,
		m_description_extra(sv_to_string(description_extra)) ,
		m_value_multiplicity(value_multiplicity) ,
		m_hidden(description.empty()||level==0U) ,
		m_value_description(sv_to_string(value_description)) ,
		m_level(level) ,
		m_main_tag(main_tag) ,
		m_tag_bits(main_tag|tag_bits)
{
	if( m_description_extra.size() == 2U )
	{
		// eg. add( Option( 'x' , "xx" , tx("do xxx! or xxxx") , "!\\" , ... ) ;
		// or: add( Option( 'x' , "xx" , "do xxx", " or xxxx" , ... ) ;
		char sep = m_description_extra.at( 0U ) ;
		char escape = m_description_extra.at( 1U ) ;
		m_description_extra.clear() ;

		StringArray sub_parts ;
		sub_parts.reserve( 2U ) ;
		Str::splitIntoFields( m_description , sub_parts , sep , escape ) ;
		if( sub_parts.size() >= 2U )
		{
			m_description = sub_parts[0] ;
			m_description_extra = sub_parts[1] ;
		}
	}
}

G::Option::Multiplicity G::Option::decode( const std::string & s )
{
	if( s == "0" )
		return Multiplicity::zero ;
	else if( s == "01" )
		return Multiplicity::zero_or_one ;
	else if( s == "1" )
		return Multiplicity::one ;
	else if( s == "2" )
		return Multiplicity::many ;
	else
		return Multiplicity::error ;
}

