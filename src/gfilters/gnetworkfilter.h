//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gnetworkfilter.h
///

#ifndef G_NETWORK_FILTER_H
#define G_NETWORK_FILTER_H

#include "gdef.h"
#include "gfilter.h"
#include "gclientptr.h"
#include "gfilestore.h"
#include "grequestclient.h"
#include "geventhandler.h"
#include "goptional.h"
#include <utility>

namespace GFilters
{
	class NetworkFilter ;
}

//| \class GFilters::NetworkFilter
/// A Filter class that passes the name of a message file to a
/// remote network server. The response of ok/abandon/fail is
/// delivered via the base class's doneSignal().
///
class GFilters::NetworkFilter : public GSmtp::Filter , private GNet::ExceptionHandler
{
public:
	NetworkFilter( GNet::EventState , GStore::FileStore & , Filter::Type ,
		const Filter::Config & , const std::string & server_location ) ;
			///< Constructor.

	~NetworkFilter() override ;
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
	void onException( GNet::ExceptionSource * , std::exception & , bool ) override ; // GNet::ExceptionHandler

public:
	NetworkFilter( const NetworkFilter & ) = delete ;
	NetworkFilter( NetworkFilter && ) = delete ;
	NetworkFilter & operator=( const NetworkFilter & ) = delete ;
	NetworkFilter & operator=( NetworkFilter && ) = delete ;

private:
	void clientEvent( const std::string & , const std::string & , const std::string & ) ;
	void sendResult( const std::string & ) ;
	void onTimeout() ;
	static bool is100( const std::string & ) ;
	static bool is45xx( const std::string & ) ;
	std::pair<std::string,int> responsePair() const ;
	static std::pair<std::string,int> responsePair( const std::string & ) ;

private:
	GNet::EventState m_es ;
	GStore::FileStore & m_file_store ;
	GNet::ClientPtr<GSmtp::RequestClient> m_client_ptr ;
	GNet::Timer<NetworkFilter> m_timer ;
	G::Slot::Signal<int> m_done_signal ;
	GNet::Location m_location ;
	G::TimeInterval m_connection_timeout ;
	G::TimeInterval m_response_timeout ;
	std::optional<std::string> m_text ;
	Result m_result {Result::fail} ;
} ;

#endif
