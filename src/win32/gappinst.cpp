//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gappinst.cpp
///

#include "gdef.h"
#include "gappinst.h"

HINSTANCE GGui::ApplicationInstance::m_hinstance = HNULL ;

GGui::ApplicationInstance::ApplicationInstance( HINSTANCE h )
{
	hinstance( h ) ;
}

void GGui::ApplicationInstance::hinstance( HINSTANCE h )
{
	if( h )
		m_hinstance = h ;
}

HINSTANCE GGui::ApplicationInstance::hinstance()
{
	return m_hinstance ;
}

