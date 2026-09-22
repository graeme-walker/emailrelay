//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gexecutablefilter.h
///

#ifndef G_EXECUTABLE_FILTER_H
#define G_EXECUTABLE_FILTER_H

#include "gdef.h"
#include "gpath.h"
#include "gfilter.h"
#include "gfilestore.h"
#include "geventhandler.h"
#include "gfutureevent.h"
#include "gtimer.h"
#include "gtask.h"
#include <utility>
#include <tuple>

namespace GFilters
{
	class ExecutableFilter ;
}

//| \class GFilters::ExecutableFilter
/// A Filter class that runs an external helper program.
///
class GFilters::ExecutableFilter : public GSmtp::Filter, private GNet::TaskCallback
{
public:
	ExecutableFilter( GNet::EventState , GStore::FileStore & , Filter::Type ,
		const Filter::Config & , const std::string & path ) ;
			///< Constructor.

	~ExecutableFilter() override ;
		///< Destructor.

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
	void onTaskDone( int , const std::string & ) override ; // GNet::TaskCallback

public:
	ExecutableFilter( const ExecutableFilter & ) = delete ;
	ExecutableFilter( ExecutableFilter && ) = delete ;
	ExecutableFilter & operator=( const ExecutableFilter & ) = delete ;
	ExecutableFilter & operator=( ExecutableFilter && ) = delete ;

private:
	static std::tuple<std::string,int,std::string> parseOutput( std::string , const std::string & ) ;
	void onTimeout() ;
	std::string prefix() const ;

private:
	GStore::FileStore & m_file_store ;
	G::Slot::Signal<int> m_done_signal ;
	Filter::Type m_filter_type ;
	Exit m_exit ;
	G::Path m_path ;
	G::TimeInterval m_timeout ;
	GNet::Timer<ExecutableFilter> m_timer ;
	std::string m_response ;
	int m_response_code {0} ;
	std::string m_reason ;
	GNet::Task m_task ;
} ;

#endif
