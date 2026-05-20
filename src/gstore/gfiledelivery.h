//
// Copyright (C) 2001-2024 Graeme Walker <graeme_walker@users.sourceforge.net>
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// ===
///
/// \file gfiledelivery.h
///

#ifndef G_FILE_DELIVERY_H
#define G_FILE_DELIVERY_H

#include "gdef.h"
#include "gmessagedelivery.h"
#include "gfilestore.h"
#include "genvelope.h"
#include "gexception.h"
#include "gstringview.h"
#include "gpath.h"
#include <fstream>
#include <utility>

namespace GStore
{
	class FileDelivery ;
}

//| \class GStore::FileDelivery
/// An implementation of the MessageDelivery interface that delivers
/// message files to mailboxes. Also provides a low-level delivery
/// function deliverTo().
///
/// The deliver() override takes a ".new" or ".busy" message from the
/// file store and delivers it to its local recipient mailbox
/// sub-directories and then deletes the original message files
/// (unless configured with 'no_delete').
///
class GStore::FileDelivery : public MessageDelivery
{
public:
	G_EXCEPTION( EnvelopeWriteError , tx("delivery: cannot write envelope file") )
	G_EXCEPTION( ContentWriteError , tx("delivery: cannot write content file") )
	G_EXCEPTION( MkdirError , tx("delivery: cannot create delivery directory") )
	G_EXCEPTION( MaildirCopyError , tx("delivery: cannot write maildir tmp file") )
	G_EXCEPTION( MaildirMoveError , tx("delivery: cannot move maildir file") )
	struct Config /// A configuration structure for GStore::FileDelivery.
	{
		bool hardlink {false} ; // copy the content by hard-linking
		bool no_delete {false} ; // don't delete the original message
		bool pop_by_name {false} ; // copy only the envelope file
		bool simple {false} ; // simplify wrt. maildir permissions
	} ;

	FileDelivery( FileStore & , const Config & ) ;
		///< Constructor. The delivery base directory is an attribute of
		///< the FileStore.

	static void deliverTo( FileStore & , std::string_view prefix ,
		const G::Path & dst_dir , const G::Path & envelope_path , const G::Path & content_path ,
		const Config & config ) ;
			///< Low-level function to copy a single message into a mailbox
			///< sub-directory or a pop-by-name sub-directory. Throws
			///< on error (incorporating the given prefix).
			///<
			///< If "config.pop_by_name" then only the envelope is copied and
			///< the given destination directory is expected to be an immediate
			///< sub-directory of the content file's directory.
			///<
			///< The content file is hard-linked if "config.hardlink".
			///<
			///< The process umask is modified when creating files so that
			///< the new files have full group access. The destination
			///< directory should normally have sticky group ownership.
			///<
			///< If the mailbox directory contains tmp/new/cur sub-directories
			///< then the delivery is a "maildir" delivery. In that case the
			///< content file (only) is copied to "tmp", then its ownership
			///< is changed to match the directory (if possible), and then
			///< it is moved to "new". Note that "config.hardlink" and
			///< "config.pop_by_name" are ignored. If "config.simple" then
			///< there is no attempt to change the file ownership.

public:
	~FileDelivery() override = default ;
	FileDelivery( const FileDelivery & ) = delete ;
	FileDelivery( FileDelivery && ) = delete ;
	FileDelivery & operator=( const FileDelivery & ) = delete ;
	FileDelivery & operator=( FileDelivery && ) = delete ;

private: // overrides
	bool deliver( const MessageId & , bool ) override ; // GStore::MessageDelivery

private:
	using FileOp = FileStore::FileOp ;
	bool deliverToMailboxes( const G::Path & , const Envelope & , const G::Path & , const G::Path & ) ;
	static G::StringArray mailboxes( const Config & , const GStore::Envelope & ) ;
	static std::string mailbox( const Config & , const std::string & ) ;
	static std::string id( const G::Path & ) ;
	static std::string hostname() ;
	G::Path cpath( const MessageId & ) const ;
	G::Path epath( const MessageId & , FileStore::State ) const ;

private:
	FileStore & m_store ;
	Config m_config ;
} ;

#endif
