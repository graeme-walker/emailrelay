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

GSmtp::FilterFactoryBase::Spec::Spec( std::string_view first_in ,
	std::string_view second_in , int verbosity_in ) :
		first(G::sv_to_string(first_in)) ,
		second(G::sv_to_string(second_in)) ,
		verbosity(verbosity_in)
{
}

