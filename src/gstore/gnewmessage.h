//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gnewmessage.h
///

#ifndef G_SMTP_NEW_MESSAGE_H
#define G_SMTP_NEW_MESSAGE_H

#include "gdef.h"
#include "gmessagestore.h"

namespace GStore
{
	class NewMessage ;
}

//| \class GStore::NewMessage
/// An abstract class to allow the creation of a new message in
/// the message store.
///
/// \code
/// auto new_msg = make_unique<NewMessageImp>( envelope_from ) ;
/// new_msg->addTo( envelope_to_1 ) ;
/// new_msg->addTo( envelope_to_2 ) ;
/// for( auto line : content )
///   new_msg->addContent( line ) ;
/// new_msg->prepare(...) ;
/// startFiltering( new_msg ) ;
/// \endcode
///
/// \see GStore::MessageStore
///
class GStore::NewMessage
{
public:
	enum class Status
	{
		Ok ,
		TooBig ,
		Error
	} ;

	virtual void addTo( const std::string & to , bool local , MessageStore::AddressStyle ) = 0 ;
		///< Adds a 'to' address.

	virtual Status addContent( const char * , std::size_t ) = 0 ;
		///< Adds a line of content, typically ending with CR-LF.
		///< Returns an error enum, but errors accumulate internally
		///< and thrown by prepare(). Adding zero bytes in order
		///< to test the current status is allowed.

	virtual void prepare( const std::string & session_auth_id ,
		const std::string & peer_socket_address , const std::string & peer_certificate ) = 0 ;
			///< Prepares to store the message in the message store.
			///< Throws on error, including any errors that accumulated
			///< while adding content.

	virtual void commit( bool throw_on_error ) = 0 ;
		///< Commits the prepare()d message to the store and disables
		///< the cleanup otherwise performed by the destructor.
		///< Either throws or ignores commit errors.

	virtual MessageId id() const = 0 ;
		///< Returns the message's unique identifier.

	virtual std::string location() const = 0 ;
		///< Returns the message's unique location.

	virtual std::size_t contentSize() const = 0 ;
		///< Returns the content size. Returns the maximum size_t value
		///< on overflow.

	void addContentLine( const std::string & ) ;
		///< A convenience function that calls addContent() taking
		///< a string parameter and adding CR-LF.

	virtual ~NewMessage() = default ;
		///< Destructor. Rolls back any prepare()d storage
		///< if un-commit()ed.
} ;

#endif
