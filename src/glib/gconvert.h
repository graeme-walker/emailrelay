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
// gconvert.h
//

#ifndef G_CONVERT_H
#define G_CONVERT_H

#include "gdef.h"
#include "gexception.h"

G_EXCEPTION( GConvertOverflow , "arithmetic overflow" ) ;

// Template function: GConvert
// Description: Does arithmetic conversions with
// overflow checking.
//
template <class Tout, class Tin>
inline
Tout GConvert( const Tin & in )
{
	Tout out = in ;
	Tin copy = out ;
	if( in != copy )
		throw GConvertOverflow( std::stringstream() << in ) ;
	return out ;
}

#endif

