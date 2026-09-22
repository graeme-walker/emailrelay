//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gsimplefilterbase.h
///

#ifndef G_SIMPLE_FILTER_BASE_H
#define G_SIMPLE_FILTER_BASE_H

#include "gdef.h"
#include "gfilter.h"
#include "gfilestore.h"
#include "genvelope.h"
#include "geventstate.h"
#include "gslot.h"
#include "gtimer.h"
#include "gstringview.h"

namespace GFilters
{
	class SimpleFilterBase ;
}

//| \class GFilters::SimpleFilterBase
/// A GSmtp::Filter base class for filters that run synchronously.
/// Concrete classes should implement run().
///
class GFilters::SimpleFilterBase : public GSmtp::Filter
{
public:
	SimpleFilterBase( GNet::EventState , Filter::Type , std::string_view id ) ;
		///< Constructor.

	virtual Result run( const GStore::MessageId & , bool & special_out ,
		GStore::FileStore::State ) = 0 ;
			///< Runs the filter synchronously and returns the result.

	std::string prefix() const ;
		///< Returns a logging prefix derived from Filter::Type and filter id.

private: // overrides
	bool quiet() const final ;
	void start( const GStore::MessageId & ) final ;
	std::string id() const final ;
	G::Slot::Signal<int> & doneSignal() noexcept final ;
	void cancel() final ;
	Result result() const final ;
	std::string response() const final ;
	int responseCode() const final ;
	std::string reason() const final ;
	bool special() const final ;

private:
	void onTimeout() ;

private:
	Filter::Type m_filter_type ;
	std::string m_id ;
	GNet::Timer<SimpleFilterBase> m_timer ;
	G::Slot::Signal<int> m_done_signal ;
	Result m_result {Result::fail} ;
	bool m_special {false} ;
} ;

#endif
