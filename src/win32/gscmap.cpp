//
// Copyright (C) 2001-2024 Graeme Walker <graeme_walker@users.sourceforge.net>
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

