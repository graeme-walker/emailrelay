//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gsimplefilterbase.cpp
///

#include "gdef.h"
#include "gsimplefilterbase.h"
#include "gfilestore.h"

GFilters::SimpleFilterBase::SimpleFilterBase( GNet::EventState es ,
	Filter::Type filter_type , std::string_view id ) :
		m_filter_type(filter_type) ,
		m_id(G::sv_to_string(id)) ,
		m_timer(*this,&SimpleFilterBase::onTimeout,es)
{
}

std::string GFilters::SimpleFilterBase::id() const
{
	return m_id ;
}

bool GFilters::SimpleFilterBase::quiet() const
{
	return false ;
}

void GFilters::SimpleFilterBase::start( const GStore::MessageId & message_id )
{
	GStore::FileStore::State e_state =
		m_filter_type == GSmtp::Filter::Type::server ?
			GStore::FileStore::State::New :
			GStore::FileStore::State::Locked ;

	m_special = false ;
	m_result = run( message_id , m_special , e_state ) ;
	m_timer.startTimer( 0U ) ;
}

G::Slot::Signal<int> & GFilters::SimpleFilterBase::doneSignal() noexcept
{
	return m_done_signal ;
}

void GFilters::SimpleFilterBase::cancel()
{
	m_timer.cancelTimer() ;
}

GSmtp::Filter::Result GFilters::SimpleFilterBase::result() const
{
	return m_result ;
}

std::string GFilters::SimpleFilterBase::response() const
{
	return { m_result == Result::fail ? "failed" : "" } ;
}

int GFilters::SimpleFilterBase::responseCode() const
{
	return 0 ;
}

std::string GFilters::SimpleFilterBase::reason() const
{
	return response() ;
}

bool GFilters::SimpleFilterBase::special() const
{
	return m_special ;
}

void GFilters::SimpleFilterBase::onTimeout()
{
	m_done_signal.emit( static_cast<int>(m_result) ) ;
}

std::string GFilters::SimpleFilterBase::prefix() const
{
	return G::sv_to_string(strtype(m_filter_type)).append(" [").append(id()).append(1U,']') ;
}

