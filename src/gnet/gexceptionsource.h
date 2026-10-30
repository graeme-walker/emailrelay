//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gexceptionsource.h
///

#ifndef G_NET_EXCEPTION_SOURCE_H
#define G_NET_EXCEPTION_SOURCE_H

#include "gdef.h"

namespace GNet
{
	class ExceptionSource ;
}

//| \class GNet::ExceptionSource
/// A mixin base class that identifies the source of an exception
/// when delivered to GNet::ExceptionHandler.
///
/// The primary motivation is to allow a Server to manage its
/// ServerPeer list when one of them throws an exception.
///
class GNet::ExceptionSource
{
public:
	virtual ~ExceptionSource() ;
		///< Destructor.

public:
	ExceptionSource() = default ;
	ExceptionSource( const ExceptionSource & ) = delete ;
	ExceptionSource( ExceptionSource && ) = delete ;
	ExceptionSource & operator=( const ExceptionSource & ) = delete ;
	ExceptionSource & operator=( ExceptionSource && ) = delete ;
} ;

#endif
