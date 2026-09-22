//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gnetdone.h
///

#ifndef G_NET_DONE_H
#define G_NET_DONE_H

#include "gdef.h"
#include "gexception.h"
#include <stdexcept>

namespace GNet
{
	class Done ;
}

//| \class GNet::Done
/// An exception class that is caught separately by GNet::EventEmitter
/// and GNet::TimerList so that onException() callbacks have their
/// 'done' parameter set.
/// \see GNet::ClientPtr
///
class GNet::Done : public std::runtime_error
{
public:
	Done() ;
		///< Constructor.
} ;

#endif
