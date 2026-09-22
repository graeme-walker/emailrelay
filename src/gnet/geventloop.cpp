//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file geventloop.cpp
///

#include "gdef.h"
#include "geventloop.h"
#include "glog.h"
#include "gassert.h"

GNet::EventLoop * GNet::EventLoop::m_this = nullptr ;

GNet::EventLoop::EventLoop()
{
	if( m_this == nullptr )
		m_this = this ;
}

GNet::EventLoop::~EventLoop()
{
	if( m_this == this )
		m_this = nullptr ;
}

GNet::EventLoop * GNet::EventLoop::ptr() noexcept
{
	return m_this ;
}

GNet::EventLoop & GNet::EventLoop::instance()
{
	if( m_this == nullptr )
		throw NoInstance() ;
	return *m_this ;
}

bool GNet::EventLoop::exists()
{
	return m_this != nullptr ;
}

void GNet::EventLoop::stop( const G::SignalSafe & signal_safe )
{
	if( m_this != nullptr )
		m_this->quit( signal_safe ) ;
}

