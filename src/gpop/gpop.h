//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gpop.h
///

#ifndef G_POP_H
#define G_POP_H

#include "gdef.h"
#include "gpopstore.h"
#include "gpopserver.h"
#include "gsecrets.h"
#include "geventstate.h"
#include "gpath.h"
#include <memory>

namespace GPop
{
	bool enabled() noexcept ;
		///< Returns true if pop code is built in.

	std::unique_ptr<Store> newStore( const G::Path & spool_dir , const Store::Config & ) ;
		///< Creates a new Pop::Store.

	std::unique_ptr<GAuth::SaslServerSecrets> newSecrets( const std::string & path ) ;
		///< Creates a new SaslServerSecrets for newStore().

	std::unique_ptr<Server> newServer( GNet::EventState , Store & ,
		const GAuth::SaslServerSecrets & , const Server::Config & ) ;
			///< Creates a new server.

	void report( const Server * , const std::string & group = {} ) ;
		///< Calls GPop::Server::report().
}

#endif

