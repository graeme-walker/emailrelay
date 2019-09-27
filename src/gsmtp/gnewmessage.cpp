//
// Copyright (C) 2001-2019 Graeme Walker <graeme_walker@users.sourceforge.net>
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
//
// gnewmessage.cpp
//

#include "gdef.h"
#include "gmessagestore.h"
#include "gnewmessage.h"
#include <iostream>

GSmtp::NewMessage::~NewMessage()
{
}

bool GSmtp::NewMessage::addTextLine( const std::string & line )
{
	return addText( line.data() , line.size() ) && addText( "\r\n" , 2U ) ;
}

/// \file gnewmessage.cpp
