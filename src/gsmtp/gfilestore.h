//
// Copyright (C) 2001-2003 Graeme Walker <graeme_walker@users.sourceforge.net>
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
// gfilestore.h
//

#ifndef G_SMTP_FILE_STORE_H
#define G_SMTP_FILE_STORE_H

#include "gdef.h"
#include "gsmtp.h"
#include "gmessagestore.h"
#include "gdatetime.h"
#include "gexception.h"
#include "gnoncopyable.h"
#include "groot.h"
#include "gpath.h"
#include <memory>
#include <string>

namespace GSmtp
{
	class FileStore ;
	class FileReader ;
	class FileWriter ;
}

// Class: GSmtp::FileStore
// Description: A concrete implementation of the MessageStore
// interface, dealing in flat files. Passes out unique sequence
// numbers, filesystem paths and i/o streams to NewMessageImp.
//
class GSmtp::FileStore : public GSmtp:: MessageStore
{
public:
	G_EXCEPTION( InvalidDirectory , "invalid spool directory" ) ;
	G_EXCEPTION( GetError , "error reading specific message" ) ;

	explicit FileStore( const G::Path & dir , bool optimise = false ) ;
		// Constructor. Throws an exception if the
		// storage directory is invalid.
		//
		// If the optimise flag is set then the implementation
		// of empty() will be efficient for an empty file
		// store (ignoring failed and local-delivery messages).
		// This might be useful for applications in which the
		// main event loop is used to check for pending jobs.
		// The disadvantage is that this process will not be
		// sensititive to messages deposited into its spool
		// directory by other processes.

	unsigned long newSeq() ;
		// Hands out a new non-zero sequence number.

	std::auto_ptr<std::ostream> stream( const G::Path & path );
		// Returns a stream to the given content.

	G::Path contentPath( unsigned long seq ) const ;
		// Returns the path for a content file.

	G::Path envelopePath( unsigned long seq ) const ;
		// Returns the path for an envelope file.

	G::Path envelopeWorkingPath( unsigned long seq ) const ;
		// Returns the path for an envelope file
		// which is in the process of being written.

	virtual bool empty() const ;
		// Returns true if there are no stored messages.

	virtual std::auto_ptr<StoredMessage> get( unsigned long id ) ;
		// Extracts a stored message.

	virtual MessageStore::Iterator iterator( bool lock ) ;
		// Returns an iterator for stored messages.

	virtual std::auto_ptr<NewMessage> newMessage( const std::string & from ) ;
		// Creates a new message in the store.

	static std::string x() ;
		// Returns the prefix for envelope header lines.

	static std::string format( int n = 0 ) ;
		// Returns an identifier for the storage format
		// implemented by this class. If n is -1 then
		// it returns the previous format (etc.).

	void updated() ;
		// Called by associated classes to trigger onEvent().

protected:
	virtual void onEvent() ;
		// Called when something might have changed. The
		// default implementation does nothing.

private:
	static void checkPath( const G::Path & dir ) ;
	G::Path fullPath( const std::string & filename ) const ;
	std::string filePrefix( unsigned long seq ) const ;
	std::string getline( std::istream & ) const ;
	std::string value( const std::string & ) const ;
	std::string crlf() const ;
	bool emptyCore() const ;

private:
	unsigned long m_seq ;
	G::Path m_dir ;
	bool m_optimise ;
	bool m_empty ; // mutable
	unsigned long m_pid_modifier ;
} ;

// Class: GSmtp::FileReader
// Description: Used by GSmtp::FileStore, GSmtp::NewFile and
// GSmtp::StoredFile to claim read permissions.
// See also: G::Root
//
class GSmtp::FileReader : public G::noncopyable
{
public:
	FileReader() ;
	~FileReader() ;
} ;

// Class: GSmtp::FileWriter
// Description: Used by GSmtp::FileStore, GSmtp::NewFile and
// GSmtp::StoredFile to claim write permissions.
// See also: G::Root
//
class GSmtp::FileWriter : public G::noncopyable
{
public:
	FileWriter() ;
	~FileWriter() ;
private:
	G::Root m_root ;
	G::Process::Umask m_umask ;
} ;

#endif
