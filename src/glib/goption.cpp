//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file goption.cpp
///

#include "gdef.h"
#include "goption.h"

G::Option::Option( char c_in , const std::string & name_in , const std::string & description_in ,
	const std::string & description_extra_in , Multiplicity value_multiplicity_in ,
	const std::string & vd_in , unsigned int level_in ) :
		c(c_in) ,
		name(name_in) ,
		description(description_in) ,
		description_extra(description_extra_in) ,
		value_multiplicity(value_multiplicity_in) ,
		hidden(description_in.empty()||level_in==0U) ,
		value_description(vd_in) ,
		level(level_in) ,
		main_tag(0U) ,
		tag_bits(0U)
{
}

G::Option::Option( char c_in , const char * name_in , const char * description_in ,
	const char * description_extra_in , Multiplicity value_multiplicity_in ,
	const char * vd_in , unsigned int level_in , unsigned int main_tag_in ,
	unsigned int tag_bits_in ) :
		c(c_in) ,
		name(name_in) ,
		description(description_in) ,
		description_extra(description_extra_in) ,
		value_multiplicity(value_multiplicity_in) ,
		hidden(*description_in=='\0'||level_in==0U) ,
		value_description(vd_in) ,
		level(level_in) ,
		main_tag(main_tag_in) ,
		tag_bits(main_tag_in|tag_bits_in)
{
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

