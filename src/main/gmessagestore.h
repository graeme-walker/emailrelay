//
// Copyright (C) 2001 Graeme Walker <graeme_walker@users.sourceforge.net>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
// ===
//
// gmessagestore.h
//

#ifndef G_SMTP_MESSAGE_STORE_H
#define G_SMTP_MESSAGE_STORE_H

#include "gdef.h"
#include "gsmtp.h"
#include "gexception.h"
#include "gstrings.h"
#include "gpath.h"

namespace GSmtp
{
	class MessageStore ;
	class StoredMessage ;
	class NewMessage ;

	class MessageStoreImp ;
	class MessageStoreIteratorImp ;
	class StoredMessageImp ;
	class NewMessageImp ;
} ;

// Class: GSmtp::NewMessage
// Description: An abstract class to allow the creation
// of a new message in the message store.
// See also: MessageStore, MessageStore::newMessage()
//
class GSmtp::NewMessage
{
public:
	virtual void addTo( const std::string & to , bool local ) = 0 ;
		// Adds a 'to' address.

	virtual void addText( const std::string & line ) = 0 ;
		// Adds a line of content.

	virtual void store() = 0 ;
		// Stores the message in the message store.

	virtual ~NewMessage() ;
		// Destructor.

private:
	void operator=( const NewMessage & ) ;
} ;

// Class: GSmtp::StoredMessage
// Description: An abstract class for messages which have
// come from the store.
// See also: MessageStore, MessageStore::get()
//
class GSmtp::StoredMessage
{
public:
	virtual const std::string & from() const = 0 ;
		// Returns the envelope 'from' field.

	virtual const G::Strings & to() const = 0 ;
		// Returns the envelope 'to' fields.

	virtual std::auto_ptr<std::istream> extractContentStream() = 0 ;
		// Extracts the content stream.
		// Can only be called once.

	virtual void destroy() = 0 ;
		// Deletes the message within the store.

	virtual void fail( const std::string & reason ) = 0 ;
		// Marks the message as failed within the store.

	virtual bool eightBit() const = 0 ;
		// Returns true if the message content (header+body)
		// contains a character with the most significant
		// bit set.

	virtual ~StoredMessage() ;
		// Destructor.

private:
	void operator=( const StoredMessage & ) ;
} ;

// Class: GSmtp::MessageStore
// Description: A singleton class which allows SMTP messages
// (envelope+content) to be stored and retrieved.
//
// The implementation puts separate envelope and content
// files in a spool directory. The content file is
// written first. The presence of a matching envelope
// file is used to indicate that the content file
// is valid and that it has been commited to the
// care of the SMTP system for delivery.
//
// See also: NewMessage, StoredMessage, ProtocolMessage
//
class GSmtp::MessageStore
{
public:
	G_EXCEPTION( InvalidDirectory , "invalid spool directory" ) ;
	G_EXCEPTION( WriteError , "error writing file" ) ;
	G_EXCEPTION( NoInstance , "no message store instance" ) ;
	G_EXCEPTION( FormatError , "format error" ) ;
	class Iterator // An iterator class for GSmtp::MessageStore.
	{
		public: std::auto_ptr<StoredMessage> next() ;
		private: MessageStoreIteratorImp * m_imp ;
		public: Iterator() ;
		public: explicit Iterator( MessageStoreIteratorImp * ) ;
		public: ~Iterator() ;
		public: Iterator( const Iterator & ) ;
		public: Iterator & operator=( const Iterator & ) ;
	} ;

	static MessageStore & instance() ;
		// Singleton access.

	static G::Path defaultDirectory() ;
		// Returns a default spool directory, such as
		// "/usr/local/var/spool/emailrelay". (Typically
		// has an os-specific implementation.)

	explicit MessageStore( const G::Path & directory ) ;
		// Constructor. Throws exceptions if
		// not a valid storage directory.

	~MessageStore() ;
		// Destructor.

	std::auto_ptr<NewMessage> newMessage( const std::string & from ) ;
		// Creates a new message.

	bool empty() const ;
		// Returns true if the message store is empty.

	std::auto_ptr<StoredMessage> get() ;
		// Pulls a message out of the store (selected
		// at random). Returns a NULL smart pointer
		// if there are no messages to extract.
		//
		// As a side effect some stored messages may be
		// marked as bad, or deleted (if they
		// have no recipients).

	Iterator iterator() ;
		// Returns a read iterator. (Note that copies of
		// iterators share state. For independent iterators
		// call iterator() for each.)
		//
		// As a side effect of iteration some stored
		// messages may be marked as bad, or deleted (if
		// they have no recipients).

	G::Path directory() const ;
		// Returns the storage directory (as passed
		// to the constructor).

private:
	MessageStore( const MessageStore & ) ;
	void operator=( const MessageStore & ) ;

private:
	static MessageStore * m_this ;
	MessageStoreImp * m_imp ;
} ;

#endif

