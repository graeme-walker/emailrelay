//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gdeliveryfilter.cpp
///

#include "gdef.h"
#include "gdeliveryfilter.h"
#include "gfiledelivery.h"
#include "gstringtoken.h"
#include "glog.h"

GFilters::DeliveryFilter::DeliveryFilter( GNet::EventState es , GStore::FileStore & store ,
	Filter::Type filter_type , const Filter::Config & filter_config , const std::string & spec ) :
		SimpleFilterBase(es,filter_type,"deliver:") ,
		m_store(store) ,
		m_filter_type(filter_type) ,
		m_filter_config(filter_config) ,
		m_spec(spec)
{
}

GSmtp::Filter::Result GFilters::DeliveryFilter::run( const GStore::MessageId & message_id ,
	bool & , GStore::FileStore::State )
{
	GStore::FileDelivery::Config config ;
	std::string_view spec = m_spec ;
	for( G::StringTokenView t( spec , ";" , 1U ) ; t ; ++t )
	{
		if( t() == "h" || t() == "hardlink" ) config.hardlink = true ;
		if( t() == "n" || t() == "no_delete" || t() == "nodelete" ) config.no_delete = true ;
		if( t() == "p" || t() == "pop" ) config.pop_by_name = true ;
	}

	GStore::FileDelivery delivery_imp( m_store , config ) ;
	GStore::MessageDelivery & delivery = delivery_imp ;
	bool removed = delivery.deliver( message_id , m_filter_type == Filter::Type::server ) ;

	return removed ? Result::abandon : Result::ok ;
}

