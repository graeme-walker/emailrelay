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
// gstoredfile.cpp
//

#include "gdef.h"
#include "gsmtp.h"
#include "gfilestore.h"
#include "gstoredfile.h"
#include "gmemory.h"
#include "gfile.h"
#include "gstr.h"
#include "glog.h"
#include "gassert.h"
#include <fstream>

GSmtp::StoredFile::StoredFile( const G::Path & path ) :
	m_envelope_path(path)
{
	G_DEBUG( "StoredFile: \"" << path << "\"" ) ;
}

GSmtp::StoredFile::~StoredFile()
{
}

bool GSmtp::StoredFile::eightBit() const
{
	return m_eight_bit ;
}

bool GSmtp::StoredFile::readEnvelope( std::string & reason , bool check )
{
	try
	{
		readEnvelopeCore( check ) ;
		return true ;
	}
	catch( std::exception & e )
	{
		reason = e.what() ;
		return false ;
	}
}

void GSmtp::StoredFile::readEnvelopeCore( bool check )
{
	std::ifstream stream( m_envelope_path.str().c_str() , std::ios_base::binary | std::ios_base::in ) ;
	if( ! stream.good() )
		throw OpenError() ;

	readFormat( stream ) ;
	readFlag( stream ) ;
	readFrom( stream ) ;
	readToList( stream ) ;

	if( check && m_to_remote.size() == 0U )
		throw NoRecipients() ;

	if( ! stream.good() )
		throw StreamError() ;
}

void GSmtp::StoredFile::readFormat( std::istream & stream )
{
	std::string format_line = getline(stream) ;
	if( value(format_line) != FileStore::format() )
		throw InvalidFormat( value(format_line) + "!=" + FileStore::format() ) ;
}

void GSmtp::StoredFile::readFlag( std::istream & stream )
{
	std::string content_line = getline(stream) ;
	m_eight_bit = value(content_line) == "8bit" ;
}

void GSmtp::StoredFile::readFrom( std::istream & stream )
{
	m_from = value(getline(stream)) ;
	G_DEBUG( "GSmtp::StoredFile::readFrom: from \"" << m_from << "\"" ) ;
}

void GSmtp::StoredFile::readToList( std::istream & stream )
{
	std::string to_count_line = getline(stream) ;
	unsigned int to_count = G::Str::toUInt( value(to_count_line) ) ;

	for( unsigned int i = 0U ; i < to_count ; i++ )
	{
		std::string to_line = getline(stream) ;
		bool is_local = to_line.find(FileStore::x()+"To-Local") == 0U ;
		bool is_remote = to_line.find(FileStore::x()+"To-Remote") == 0U ;
		if( ! is_local && ! is_remote )
			throw InvalidTo(to_line) ;
			
		G_DEBUG( "GSmtp::StoredFile::readToList: to "
			"[" << (i+1U) << "/" << to_count << "] "
			"(" << (is_local?"local":"remote") << ") "
			<< "\"" << value(to_line) << "\"" ) ;

		if( is_local )
			m_to_local.push_back( value(to_line) ) ;
		else
			m_to_remote.push_back( value(to_line) ) ;
	}
}

void GSmtp::StoredFile::readEnd( std::istream & stream )
{
	std::string end = getline(stream) ;
	if( end.find(FileStore::x()+"End") != 0U )
		throw NoEnd() ;
}

bool GSmtp::StoredFile::openContent( std::string & reason )
{
	try
	{
		G::Path content_path = contentPath() ;
		G_DEBUG( "GSmtp::FileStore::openContent: \"" << content_path << "\"" ) ;
		std::auto_ptr<std::istream> stream( new std::ifstream(
			content_path.str().c_str() , std::ios_base::in | std::ios_base::binary ) ) ;
		if( !stream->good() )
		{
			reason = "cannot open content file" ;
			return false ;
		}

		G_LOG( "GSmtp::MessageStore: processing envelope \"" << m_envelope_path.basename() << "\"" ) ;
		G_LOG( "GSmtp::MessageStore: processing content \"" << content_path.basename() << "\"" ) ;

		m_content = stream ;
		return true ;
	}
	catch( std::exception & e )
	{
		G_WARNING( "GSmtp::FileStore: exception: " << e.what() ) ;
		reason = e.what() ;
		return false ;
	}
}

std::string GSmtp::StoredFile::getline( std::istream & stream ) const
{
	return G::Str::readLineFrom( stream , crlf() ) ;
}

std::string GSmtp::StoredFile::value( const std::string & s ) const
{
	size_t pos = s.find(' ') ;
	if( pos == std::string::npos )
		throw MessageStore::FormatError() ;
	return s.substr(pos+1U) ;
}

std::string GSmtp::StoredFile::crlf() const
{
	return std::string( "\015\012" ) ;
}

bool GSmtp::StoredFile::lock()
{
	G::Path & src = m_envelope_path ;
	G::Path dst( src.str() + ".busy" ) ;
	bool ok = G::File::rename( src , dst , G::File::NoThrow() ) ;
	if( ok )
	{
		G_LOG( "GSmtp::StoredMessage: locking file \"" << src.basename() << "\"" ) ;
		m_envelope_path = dst ;
	}
	return ok ;
}

void GSmtp::StoredFile::fail( const std::string & reason )
{
	try
	{
		// write the reason into the file
		{
			std::ofstream file( m_envelope_path.str().c_str() ,
				std::ios_base::binary | std::ios_base::ate ) ;
			file << FileStore::x() << "Reason: " << reason ;
		}

		G::Path env_temp( m_envelope_path ) ; // "foo.envelope.busy"
		env_temp.removeExtension() ; // "foo.envelope"
		G::Path bad( env_temp.str() + ".bad" ) ; // "foo.envelope.bad"
		G_LOG( "GSmtp::StoredMessage: failing file: "
			<< "\"" << m_envelope_path.basename() << "\" -> "
			<< "\"" << bad.basename() << "\"" ) ;

		G::File::rename( m_envelope_path , bad , G::File::NoThrow() ) ;
	}
	catch(...)
	{
	}
}

void GSmtp::StoredFile::destroy()
{
	try
	{
		G_LOG( "GSmtp::StoredMessage: deleting file: \"" << m_envelope_path.basename() << "\"" ) ;
		G::File::remove( m_envelope_path , G::File::NoThrow() ) ;

		G::Path content_path = contentPath() ;
		G_LOG( "GSmtp::StoredMessage: deleting file: \"" << content_path.basename() << "\"" ) ;
		m_content <<= 0 ; // close it first
		G::File::remove( content_path , G::File::NoThrow() ) ;
	}
	catch(...)
	{
	}
}

const std::string & GSmtp::StoredFile::from() const
{
	return m_from ;
}

const G::Strings & GSmtp::StoredFile::to() const
{
	return m_to_remote ;
}

std::auto_ptr<std::istream> GSmtp::StoredFile::extractContentStream()
{
	G_ASSERT( m_content.get() != NULL ) ;
	return m_content ;
}

G::Path GSmtp::StoredFile::contentPath() const
{
	G::Path path( m_envelope_path ) ; // "foo.envelope.busy"
	path.removeExtension() ; // "foo.envelope"
	path.setExtension( "content" ) ; // "foo.content"
	return path ;
}

size_t GSmtp::StoredFile::remoteRecipientCount() const
{
	return m_to_remote.size() ;
}

