//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gcopyfilter.h
///

#ifndef G_COPY_FILTER_H
#define G_COPY_FILTER_H

#include "gdef.h"
#include "gsimplefilterbase.h"
#include "gfilestore.h"
#include "gfiledelivery.h"
#include "genvelope.h"
#include "gexception.h"

namespace GFilters
{
	class CopyFilter ;
}

//| \class GFilters::CopyFilter
/// A concrete GSmtp::Filter class that copies the message to all
/// pre-existing sub-directories of the spool directory. This is
/// similar to the 'emailrelay-filter-copy' utility.
///
class GFilters::CopyFilter : public SimpleFilterBase
{
public:
	G_EXCEPTION( Error , tx("copy filter failed to copy message files into sub-directory") )

	CopyFilter( GNet::EventState es , GStore::FileStore & ,
		Filter::Type , const Filter::Config & , const std::string & spec ) ;
			///< Constructor.

private: // overrides
	Result run( const GStore::MessageId & , bool & , GStore::FileStore::State ) override ;

private:
	using FileOp = GStore::FileStore::FileOp ;
	GStore::FileStore & m_store ;
	Filter::Config m_filter_config ;
	std::string m_spec ;
	GStore::FileDelivery::Config m_delivery_config ;
} ;

#endif
