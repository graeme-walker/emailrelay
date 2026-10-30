//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gdnsbl.h
///

#ifndef G_NET_DNSBL_H
#define G_NET_DNSBL_H

#include "gdef.h"
#include "geventstate.h"
#include "gaddress.h"
#include "gstringview.h"
#include <functional>
#include <memory>

namespace GNet
{
	class Dnsbl ;
	class DnsblImp ;
}

//| \class GNet::Dnsbl
/// A minimal bridge to GNet::DnsBlock
///
class GNet::Dnsbl
{
public:
	Dnsbl( std::function<void(bool)> callback , EventState , std::string_view config = {} ) ;
		///< Constructor. See DnsBlock::DnsBlock().

	~Dnsbl() ;
		///< Destructor.

	void start( const Address & ) ;
		///< Starts an asychronous check on the given address. The result
		///< is delivered via the callback function passed to the ctor.

	bool busy() const ;
		///< Returns true after start() and before the completion callback.

	static void checkConfig( const std::string & ) ;
		///< See DnsBlock::checkConfig().

public:
	Dnsbl( const Dnsbl & ) = delete ;
	Dnsbl( Dnsbl && ) = delete ;
	Dnsbl & operator=( const Dnsbl & ) = delete ;
	Dnsbl & operator=( Dnsbl && ) = delete ;

private:
	std::function<void(bool)> m_callback ;
	std::unique_ptr<DnsblImp> m_imp ;
} ;

#endif
