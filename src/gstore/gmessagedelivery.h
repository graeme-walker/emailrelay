//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gmessagedelivery.h
///

#ifndef G_MESSAGE_DELIVERY_H
#define G_MESSAGE_DELIVERY_H

#include "gdef.h"
#include "gmessagestore.h"
#include <string>

namespace GStore
{
	class MessageDelivery ;
}

//| \class GStore::MessageDelivery
/// An interface to deliver a message to its local recipients' mailboxes.
///
/// This interface is typically used to implement a delivery filter:
/// \code
/// struct Filter : GSmtp::Filter {
///   void start( MessageId ) override ;
///   MessageDelivery* m_delivery ;
///   ...
/// } ;
/// void Filter::start( MessageId id )
/// {
///   assert( m_filter_type == Filter::Type::server ) ;
///   m_delivery->deliver( id ) ;
///   m_timer.startTimer( 0 ) ;
/// }
/// \endcode
///
class GStore::MessageDelivery
{
public:
	virtual bool deliver( const MessageId & , bool is_new ) = 0 ;
		///< Delivers a new or locked message to its local recipients'
		///< mailboxes. Does nothing if there are no local recipients.
		///< If all the recipients are local then the message might
		///< be removed from the store. Returns true iff the message
		///< has been removed.

	virtual ~MessageDelivery() = default ;
		///< Destructor.
} ;

#endif
