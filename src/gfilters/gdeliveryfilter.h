//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gdeliveryfilter.h
///

#ifndef G_DELIVERY_FILTER_H
#define G_DELIVERY_FILTER_H

#include "gdef.h"
#include "gfilter.h"
#include "gsimplefilterbase.h"
#include "gfilestore.h"
#include "geventstate.h"

namespace GFilters
{
	class DeliveryFilter ;
}

//| \class GFilters::DeliveryFilter
/// A concrete GSmtp::Filter class that copies the message to multiple
/// spool sub-directories according to the envelope recipient list.
/// The implementation delegates to GStore::FileDelivery.
///
class GFilters::DeliveryFilter : public SimpleFilterBase
{
public:
	DeliveryFilter( GNet::EventState es , GStore::FileStore & ,
		Filter::Type , const Filter::Config & , const std::string & spec ) ;
			///< Constructor.

private: // overrides
	Result run( const GStore::MessageId & , bool & , GStore::FileStore::State ) override ;

private:
	GStore::FileStore & m_store ;
	Filter::Type m_filter_type ;
	Filter::Config m_filter_config ;
	std::string m_spec ;
} ;

#endif
