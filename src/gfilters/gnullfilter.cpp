//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gnullfilter.cpp
///

#include "gdef.h"
#include "gnullfilter.h"
#include "gassert.h"
#include "gstr.h"
#include "glog.h"

GFilters::NullFilter::NullFilter( GNet::EventState es , GStore::FileStore & ,
	Filter::Type filter_type , const Filter::Config & filter_config ) :
		m_filter_config(filter_config) ,
		m_exit(0,filter_type) ,
		m_id("exit") ,
		m_verbosity(-2) ,
		m_timeout(0) ,
		m_timer(*this,&NullFilter::onTimeout,es)
{
}

GFilters::NullFilter::NullFilter( GNet::EventState es , GStore::FileStore & ,
	Filter::Type filter_type , const Filter::Config & filter_config ,
	const std::string & spec ) :
		m_filter_config(filter_config) ,
		m_exit(G::Str::toInt(G::LogOutput::noVerbosity(spec)),filter_type) ,
		m_id("exit:"+G::LogOutput::noVerbosity(spec)) ,
		m_verbosity(G::LogOutput::parseVerbosity(spec)) ,
		m_timeout(0) ,
		m_timer(*this,&NullFilter::onTimeout,es)
{
}

GFilters::NullFilter::NullFilter( GNet::EventState es , GStore::FileStore & ,
	Filter::Type filter_type , const Filter::Config & filter_config ,
	G::TimeInterval sleep_time ) :
		m_filter_config(filter_config) ,
		m_exit(0,filter_type) ,
		m_id("sleep") ,
		m_verbosity(-2) ,
		m_timeout(sleep_time) ,
		m_timer(*this,&NullFilter::onTimeout,es)
{
	G_ASSERT( m_exit.ok() ) ;
}

std::string GFilters::NullFilter::id() const
{
	return m_id ;
}

int GFilters::NullFilter::verbosity() const noexcept
{
	return m_verbosity ;
}

bool GFilters::NullFilter::special() const
{
	return m_exit.special ;
}

GSmtp::Filter::Result GFilters::NullFilter::result() const
{
	return m_exit.result ;
}

std::string GFilters::NullFilter::response() const
{
	return ( m_exit.ok() || m_exit.abandon() ) ? std::string() : std::string("rejected") ;
}

int GFilters::NullFilter::responseCode() const
{
	return 0 ;
}

std::string GFilters::NullFilter::reason() const
{
	return ( m_exit.ok() || m_exit.abandon() ) ? std::string() : m_id ;
}

G::Slot::Signal<int> & GFilters::NullFilter::doneSignal() noexcept
{
	return m_done_signal ;
}

void GFilters::NullFilter::cancel()
{
}

void GFilters::NullFilter::start( const GStore::MessageId & )
{
	m_timer.startTimer( m_timeout ) ;
}

void GFilters::NullFilter::onTimeout()
{
	m_done_signal.emit( static_cast<int>(m_exit.result) ) ;
}

