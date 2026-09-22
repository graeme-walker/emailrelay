//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gfilterchain.h
///

#ifndef G_FILTER_CHAIN_H
#define G_FILTER_CHAIN_H

#include "gdef.h"
#include "gfilter.h"
#include "gfilterfactory.h"
#include "gtimer.h"
#include "gslot.h"
#include "geventstate.h"
#include <memory>
#include <vector>

namespace GFilters
{
	class FilterChain ;
}

//| \class GFilters::FilterChain
/// A Filter class that runs a sequence of sub-filters. The sub-filters are run
/// in sequence only as long as they return success.
///
class GFilters::FilterChain : public GSmtp::Filter
{
public:
	FilterChain( GNet::EventState , GSmtp::FilterFactoryBase & , Filter::Type ,
		const Filter::Config & , const GSmtp::FilterFactoryBase::Spec & spec ) ;
			///< Constructor.

	~FilterChain() override ;
		///< Destructor.

public:
	FilterChain( const FilterChain & ) = delete ;
	FilterChain( FilterChain && ) = delete ;
	FilterChain & operator=( const FilterChain & ) = delete ;
	FilterChain & operator=( FilterChain && ) = delete ;

private: // overrides
	std::string id() const override ; // GSmtp::Filter
	bool quiet() const override ; // GSmtp::Filter
	G::Slot::Signal<int> & doneSignal() noexcept override ; // GSmtp::Filter
	void start( const GStore::MessageId & ) override ; // GSmtp::Filter
	void cancel() override ; // GSmtp::Filter
	Result result() const override ; // GSmtp::Filter
	std::string response() const override ; // GSmtp::Filter
	int responseCode() const override ; // GSmtp::Filter
	std::string reason() const override ; // GSmtp::Filter
	bool special() const override ; // GSmtp::Filter

private:
	void add( GNet::EventState , GSmtp::FilterFactoryBase & , Filter::Type ,
		const Filter::Config & , const GSmtp::FilterFactoryBase::Spec & ) ;
	void onFilterDone( int ) ;

private:
	G::Slot::Signal<int> m_done_signal ;
	std::string m_filter_id ;
	std::vector<std::unique_ptr<GSmtp::Filter>> m_filters ;
	std::size_t m_filter_index {0U} ;
	GSmtp::Filter * m_filter {nullptr} ;
	bool m_running {false} ;
	GStore::MessageId m_message_id ;
} ;

#endif
