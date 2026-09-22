//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file geventlogging.cpp
///

#include "gdef.h"
#include "geventlogging.h"
#include "gassert.h"

GNet::EventLogging::EventLogging( const EventLogging * next ) :
	m_next(next)
{
	G_ASSERT( next != this ) ;
}

GNet::EventLogging::~EventLogging()
= default ;

std::string_view GNet::EventLogging::eventLoggingString() const
{
	return {} ;
}

const GNet::EventLogging * GNet::EventLogging::next() const noexcept
{
	return m_next ;
}

