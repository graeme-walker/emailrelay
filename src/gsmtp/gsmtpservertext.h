//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gsmtpservertext.h
///

#ifndef G_SMTP_SERVER_TEXT_H
#define G_SMTP_SERVER_TEXT_H

#include "gdef.h"
#include "gsmtpserverprotocol.h"
#include "gaddress.h"
#include <string>

namespace GSmtp
{
	class ServerText ;
}

//| \class GSmtp::ServerText
/// A default implementation of the GSmtp::ServerProtocol::Text interface.
///
class GSmtp::ServerText : public GSmtp::ServerProtocol::Text
{
public:
	ServerText( const std::string & code_ident , bool anonymous , bool with_received_line ,
		const std::string & greeting_and_receivedline_domain , const GNet::Address & peer_address ) ;
			///< Constructor.

	static std::string receivedLine( const std::string & smtp_peer_name ,
		const std::string & peer_address , const std::string & receivedline_domain ,
		bool authenticated , bool secure , const std::string & , const std::string & cipher_in ) ;

public:
	~ServerText() override = default ;
	ServerText( const ServerText & ) = default ;
	ServerText( ServerText && ) = default ;
	ServerText & operator=( const ServerText & ) = default ;
	ServerText & operator=( ServerText && ) = default ;

private: // overrides:
	std::string greeting() const override ; // Override from GSmtp::ServerProtocol::Text.
	std::string hello( const std::string & smtp_peer_name_from_helo ) const override ; // Override from GSmtp::ServerProtocol::Text.
	std::string received( const std::string & , bool , bool , const std::string & , const std::string & ) const override ; // Override from GSmtp::ServerProtocol::Text.

private:
	std::string m_code_ident ;
	bool m_anonymous ;
	bool m_with_received_line ;
	std::string m_domain ; // greeting and receivedline
	GNet::Address m_peer_address ;
} ;

#endif
