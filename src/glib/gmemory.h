//
// Copyright (C) 2001 Graeme Walker <graeme_walker@users.sourceforge.net>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
// ===
//
// gmemory.h
//

#ifndef G_MEMORY_H
#define G_MEMORY_H

#include "gdef.h"
#include <memory>

// Template function: operator<<=
// Description: A fix for the problem of resetting
// an auto_ptr portably. MSVC6.0 does not have a reset
// method, and GCC has a non-const assignment
// operators. This means that the MSVC code and
// the gcc code for resetting an auto_ptr are
// radically different. This operator hides
// those differences.
//
template <class T>
void operator<<=( std::auto_ptr<T> & ap , T * p )
{
 #ifdef G_WINDOWS
	ap = std::auto_ptr<T>( p ) ;
 #else
	ap.reset( p ) ;
 #endif
}

// Template function: operator<<=
// Description: A version for null pointers.
//
template <class T>
void operator<<=( std::auto_ptr<T> & ap , int null_pointer )
{
	//operator<<=<T>( ap , (T*)(0) ) ;
	T * p = 0 ;
	ap <<= p ;
}

#endif
