//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gverifierfactorybase.cpp
///

#include "gdef.h"
#include "gverifierfactorybase.h"

GSmtp::VerifierFactoryBase::Spec::Spec()
= default ;

GSmtp::VerifierFactoryBase::Spec::Spec( std::string_view first_in , std::string_view second_in ) :
	first(G::sv_to_string(first_in)) ,
	second(G::sv_to_string(second_in))
{
}

