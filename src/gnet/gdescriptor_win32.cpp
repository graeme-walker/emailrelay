//
// Copyright (C) 2001-2007 Graeme Walker <graeme_walker@users.sourceforge.net>
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
// gdescriptor_win32.cpp
//

#include "gdef.h"
#include "gnet.h"
#include "gdescriptor.h"

bool GNet::Descriptor__valid( Descriptor fd )
{
	return fd != INVALID_SOCKET ;
}

GNet::Descriptor GNet::Descriptor__invalid()
{
	return INVALID_SOCKET ;
}

/// \file gdescriptor_win32.cpp
