//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gnullfilter.h
///

#ifndef G_NULL_FILTER_H
#define G_NULL_FILTER_H

#include "gdef.h"
#include "gtimer.h"
#include "gfilter.h"
#include "gfilestore.h"
#include "gdatetime.h"

namespace GFilters
{
	class NullFilter ;
}

//| \class GFilters::NullFilter
/// A Filter class that does nothing.
///
class GFilters::NullFilter : public GSmtp::Filter
{
public:
	NullFilter( GNet::EventState , GStore::FileStore & ,
		Filter::Type , const Filter::Config & ) ;
			///< Constructor for a do-nothing filter.

	NullFilter( GNet::EventState , GStore::FileStore & ,
		Filter::Type , const Filter::Config & , unsigned int exit_code ) ;
			///< Constructor for a filter that behaves like an
			///< executable that always exits with the given
			///< exit code.

	NullFilter( GNet::EventState , GStore::FileStore & ,
		Filter::Type , const Filter::Config & , G::TimeInterval ) ;
			///< Constructor for a do-nothing filter that takes
			///< its time.

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

public:
	~NullFilter() override = default ;
	NullFilter( const NullFilter & ) = delete ;
	NullFilter( NullFilter && ) = delete ;
	NullFilter & operator=( const NullFilter & ) = delete ;
	NullFilter & operator=( NullFilter && ) = delete ;

private:
	void onTimeout() ;

private:
	std::string m_id ;
	Filter::Exit m_exit ;
	bool m_quiet ;
	G::TimeInterval m_timeout ;
	GNet::Timer<NullFilter> m_timer ;
	G::Slot::Signal<int> m_done_signal ;
} ;

#endif
