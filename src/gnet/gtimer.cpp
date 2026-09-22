//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gtimer.cpp
///

#include "gdef.h"
#include "gtimer.h"
#include "gtimerlist.h"
#include "gevent.h"
#include "glog.h"
#include "gassert.h"
#include <algorithm>

GNet::TimerBase::TimerBase( EventState es ) :
	m_time(G::TimerTime::zero())
{
	TimerList::instance().add( *this , es ) ;
}

GNet::TimerBase::~TimerBase()
{
	try
	{
		if( TimerList::ptr() != nullptr )
			TimerList::ptr()->remove( *this ) ;
	}
	catch(...) // dtor
	{
	}
}

bool GNet::TimerBase::expired( G::TimerTime & now ) const
{
	if( !m_active )
	{
		return false ;
	}
	else if( immediate() )
	{
		return true ;
	}
	else
	{
		// lazy evaluation of caller's idea of now -- no call
		// to TimerTime::now() if there is a zero-length
		// timer or no timers at all
		if( now == G::TimerTime::zero() )
			now = G::TimerTime::now() ;

		return m_time <= now ;
	}
}

void GNet::TimerBase::startTimer( unsigned int time , unsigned int time_us )
{
	m_active = true ;
	m_immediate = time == 0U && time_us == 0U ;
	m_time = m_immediate ? G::TimerTime::zero() : ( G::TimerTime::now() + G::TimeInterval(time,time_us) ) ;
	TimerList::instance().updateOnStart( *this ) ; // adjust()
}

void GNet::TimerBase::startTimer( const G::TimeInterval & i )
{
	m_active = true ;
	m_immediate = i == G::TimeInterval(0U) ;
	m_time = m_immediate ? G::TimerTime::zero() : ( G::TimerTime::now() + i ) ;
	TimerList::instance().updateOnStart( *this ) ; // adjust()
}

bool GNet::TimerBase::immediate() const
{
	return m_immediate ;
}

void GNet::TimerBase::adjust( unsigned long order )
{
	G_ASSERT( m_active && m_immediate ) ;
	m_time += G::TimeInterval( 0 , order ) ;
}

void GNet::TimerBase::cancelTimer()
{
	if( m_active )
	{
		m_active = false ;
		TimerList::instance().updateOnCancel( *this ) ;
	}
}

void GNet::TimerBase::doTimeout()
{
	G_ASSERT( m_active ) ;
	m_active = false ;
	onTimeout() ;
}

G::TimerTime GNet::TimerBase::t() const
{
	return m_time ;
}

