//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gscmap.cpp
///

#include "gdef.h"
#include "gscmap.h"
#include "glog.h"
#include "gassert.h"

GGui::SubClassMap::SubClassMap()
= default ;

void GGui::SubClassMap::add( HWND hwnd , SubClassMap::Proc proc , void *context )
{
	for( auto & item : m_list )
	{
		if( item.hwnd == HNULL || item.hwnd == hwnd )
		{
			item = Slot( proc , hwnd , context ) ;
			return ;
		}
	}
	m_list.emplace_back( proc , hwnd , context ) ;
}

GGui::SubClassMap::Proc GGui::SubClassMap::find( HWND hwnd , void **context_p )
{
	if( context_p != nullptr )
		*context_p = nullptr ;

	for( const auto & item : m_list )
	{
		if( item.hwnd == hwnd )
		{
			if( context_p != nullptr )
				*context_p = item.context ;
			return item.proc ;
		}
	}
	return nullptr ;
}

void GGui::SubClassMap::remove( HWND hwnd )
{
	for( auto & item : m_list )
	{
		if( item.hwnd == hwnd )
			item.hwnd = HNULL ;
	}
}

