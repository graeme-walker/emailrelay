//
// Copyright (C) 2001-2002 Graeme Walker <graeme_walker@users.sourceforge.net>
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
// gcredentials.h
//

#ifndef G_CREDENTIALS_H
#define G_CREDENTIALS_H

#include "gdef.h"

namespace G
{

// Class: credentials
// Description: A class template which can be used to provide
// controlled access to public methods in a class. Note that
// all the constructors are private, but the template-parameter
// class is declared as a friend. (Because all the methods
// are private you will not see much in the doxygen output;
// have a look at the header.)
//
// Usage:
/// struct Foo
/// {
///    void methodForBar( const credentials<Bar> & ) ;
/// } ;
///
/// void Bar::fn()
/// {
///    Foo foo ;
///    foo.methodForBar( "" ) ;
/// }
//
template <class T>
class credentials
{
	friend T ;

	credentials() ;
		// Private default constructor.

	credentials( const char * ) ;
		// Implicit private constructor.
} ;

template <class T>
inline
credentials<T>::credentials()
{
}

template <class T>
inline
credentials<T>::credentials( const char * )
{
}

} ;

#endif

