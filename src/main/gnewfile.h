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
// gnewfile.h
//

#ifndef G_SMTP_NEW_FILE_H
#define G_SMTP_NEW_FILE_H

#include "gdef.h"
#include "gsmtp.h"
#include "gfilestore.h"
#include "gnewmessage.h"
#include "gexception.h"
#include <iostream>

namespace GSmtp
{
	class NewFile ;
} ;

// Class: GSmtp::NewFile
// Description: A concrete derived class implementing the
// NewMessage interface. Writes itself to the i/o streams
// supplied by MessageStoreImp.
//
class GSmtp::NewFile : public GSmtp:: NewMessage
{
public:
	G_EXCEPTION( InvalidPath , "invalid path -- must be absolute" ) ;
	G_EXCEPTION( Dangerous , "message filtering not allowed if running as a privileged process" ) ;

	NewFile( const std::string & from , FileStore & store ) ;
	virtual ~NewFile() ;
	virtual void addTo( const std::string & to , bool local ) ;
	virtual void addText( const std::string & line ) ;
	virtual void store() ;
	virtual unsigned long id() const ;

	static void setPreprocessor( const G::Path & exe ) ;
		// Defines a program which is used for pre-processing
		// messages before they are stored.

private:
	FileStore & m_store ;
	unsigned long m_seq ;
	std::string m_from ;
	G::Strings m_to_local ;
	G::Strings m_to_remote ;
	std::auto_ptr<std::ostream> m_content ;
	G::Path m_content_path ;
	bool m_eight_bit ;
	static bool m_preprocess ;
	static G::Path m_preprocessor ;

private:
	bool saveEnvelope( std::ostream & stream , const std::string & where ) const ;
	std::string crlf() const ;
	static bool isEightBit( const std::string & line ) ;
	void deliver( const G::Strings & , const G::Path & , const G::Path & , const G::Path & ) ;
	bool preprocess( const G::Path & ) ;
} ;

#endif
