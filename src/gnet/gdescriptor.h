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
// gdescriptor.h
//

#ifndef G_DESCRIPTOR_H
#define G_DESCRIPTOR_H

#include "gdef.h"
#include "gnet.h"

namespace GNet
{
	typedef ::SOCKET Descriptor ; // (SOCKET is defined in gnet.h)

	bool Descriptor__valid( Descriptor fd ) ;
		// Tests whether the given network descriptor is valid.

	Descriptor Descriptor__invalid() ;
		// Returns an invalid network descriptor.
}

#endif

