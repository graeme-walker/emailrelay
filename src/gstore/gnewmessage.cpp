//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gnewmessage.cpp
///

#include "gdef.h"
#include "gmessagestore.h"
#include "gnewmessage.h"
#include <iostream>

void GStore::NewMessage::addContentLine( const std::string & line )
{
	addContent( line.data() , line.size() ) ;
	addContent( "\r\n" , 2U ) ;
}

