//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gmessageidfilter.h
///

#ifndef G_MESSAGE_ID_FILTER_H
#define G_MESSAGE_ID_FILTER_H

#include "gdef.h"
#include "gsimplefilterbase.h"
#include "gfilestore.h"
#include "gstringview.h"
#include "gexception.h"

namespace GFilters
{
	class MessageIdFilter ;
}

//| \class GFilters::MessageIdFilter
/// A filter that adds a RFC-822 Message-ID to the message content if
/// it does not have one already.
///
class GFilters::MessageIdFilter : public SimpleFilterBase
{
public:
	G_EXCEPTION( Error , tx("failed to add message id to content file") )

	MessageIdFilter( GNet::EventState , GStore::FileStore & ,
		Filter::Type , const Filter::Config & , const std::string & spec ) ;
			///< Constructor.

	static std::string process( const G::Path & , const std::string & domain ) ;
		///< Edits a content file by adding a message-id if necessary.
		///< Returns an error message on error.

private: // overrides
	Result run( const GStore::MessageId & , bool & , GStore::FileStore::State ) override ;

private:
	static bool isId( std::string_view ) noexcept ;
	static std::string newId( const std::string & ) ;

private:
	GStore::FileStore & m_store ;
	std::string m_domain ;
} ;

#endif
