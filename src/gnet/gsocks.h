//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gsocks.h
///

#ifndef G_NET_SOCKS_H
#define G_NET_SOCKS_H

#include "gdef.h"
#include "greadwrite.h"
#include "glocation.h"
#include "gexception.h"
#include <string>

namespace GNet
{
	class Socks ;
}

//| \class GNet::Socks
/// Implements the SOCKS4a proxy connection protocol.
///
class GNet::Socks
{
public:
	G_EXCEPTION( SocksError , tx("socks error") )

	explicit Socks( const Location & ) ;
		///< Constructor.

	bool send( G::ReadWrite & ) ;
		///< Sends the connect-request pdu using the given
		///< file descriptor. Returns true if fully sent.

	bool read( G::ReadWrite & ) ;
		///< Reads the response using the given file descriptor.
		///< Returns true if fully received and positive.
		///< Throws if the response is negative.

	static std::string buildPdu( const std::string & far_host , unsigned int far_port ) ;
		///< Builds a SOCKS4a connect request pdu.

private:
	std::size_t m_request_offset {0U} ;
	std::string m_request ;
	std::string m_response ;
} ;

#endif
