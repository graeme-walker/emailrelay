//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gprotocolmessage.cpp
///

#include "gdef.h"
#include "gprotocolmessage.h"
#include "gmessagestore.h"

void GSmtp::ProtocolMessage::addContentLine( const std::string & line )
{
	addContent( line.data() , line.size() ) ;
	addContent( "\r\n" , 2U ) ;
}

GSmtp::ProtocolMessage::ToInfo::ToInfo( const VerifierStatus & status_in ) :
	status(status_in) ,
	address_style(GStore::MessageStore::addressStyle(status_in.address))
{
}

