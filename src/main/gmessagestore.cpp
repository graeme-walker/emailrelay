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
// gmessagestore.cpp
//

#include "gdef.h"
#include "gsmtp.h"
#include "gpid.h"
#include "gdirectory.h"
#include "gmessagestore.h"
#include "gmemory.h"
#include "gpath.h"
#include "gfile.h"
#include "gstr.h"
#include "glog.h"
#include "gassert.h"
#include <iostream>
#include <fstream>

// Class: MessageStoreImp
// Description: Pimple-pattern implementation for MessageStore.
// Passes out unique sequence numbers, filesystem paths and
// i/o streams to NewMessageImp.
//
class GSmtp::MessageStoreImp
{
private:
	G::Path m_dir ;
	unsigned long m_seq ;
public:
	MessageStoreImp( const G::Path & dir ) ;

 	// new message...
	static void checkPath( const G::Path & ) ;
	const G::Path & dir() const ;
	unsigned long newSeq() ;
	std::auto_ptr<std::ostream> stream( const G::Path & path );
	G::Path contentPath( unsigned long seq ) const ;
	G::Path envelopePath( unsigned long seq ) const ;
	G::Path envelopeWorkingPath( unsigned long seq ) const ;

	// stored message...
	bool empty() const ;
	std::auto_ptr<StoredMessage> get() ;
	MessageStore::Iterator iterator() ;

	// both...
	static std::string x() ;
	static std::string format() ;
private:
	G::Path fullPath( const std::string & filename ) const ;
	std::string filePrefix( unsigned long seq ) const ;
	std::string getline( std::istream & ) const ;
	std::string value( const std::string & ) const ;
	std::string crlf() const ;
} ;

// ===

// Class: StoredMessageImp
// Description: A concete derived class implementing the
// StoredMessage (parent) and Callback (grandparent) interfaces.
//
class GSmtp::StoredMessageImp : public GSmtp::StoredMessage
{
public:
	G_EXCEPTION( InvalidFormat , "invalid format field in envelope" ) ;
	G_EXCEPTION( NoEnd , "invalid envelope file: no end marker" ) ;
	G_EXCEPTION( InvalidTo , "invalid 'to' line in envelope file" ) ;
	G_EXCEPTION( NoRecipients , "no remote recipients" ) ;
	G_EXCEPTION( OpenError , "cannot open the envelope" ) ;
	G_EXCEPTION( StreamError , "envelope reading/parsing error" ) ;
	explicit StoredMessageImp( const G::Path & envelope_path ) ;
	virtual ~StoredMessageImp() ;
	bool lock() ;
	bool readEnvelope( std::string & reason ) ;
	bool readEnvelopeCore( std::string & reason ) ;
	bool openContent( std::string & reason ) ;
	virtual bool eightBit() const ;
	virtual const std::string & from() const ;
	virtual const G::Strings & to() const ;
	virtual void destroy() ;
	virtual void fail( const std::string & reason ) ;
	virtual std::auto_ptr<std::istream> extractContentStream() ;

private:
	StoredMessageImp( const StoredMessageImp & ) ;
	void operator=( const StoredMessageImp & ) ;
	std::string crlf() const ;
	std::string getline( std::istream & stream ) const ;
	std::string value( const std::string & s ) const ;
	G::Path contentPath() const ;

private:
	G::Strings m_to_local ;
	G::Strings m_to_remote ;
	std::string m_from ;
	G::Path m_envelope_path ;
	std::auto_ptr<std::istream> m_content ;
	bool m_eight_bit ;
} ;

// ===

// Class: NewMessageImp
// Description: A concrete derived class implementing the
// NewMessage interface. Writes itself to the i/o streams
// supplied by MessageStoreImp.
//
class GSmtp::NewMessageImp : public GSmtp::NewMessage
{
public:
	NewMessageImp( const std::string & from , MessageStoreImp & store ) ;
	virtual ~NewMessageImp() ;
	virtual void addTo( const std::string & to , bool local ) ;
	virtual void addText( const std::string & line ) ;
	virtual void store() ;
private:
	MessageStoreImp & m_store ;
	unsigned long m_seq ;
	std::string m_from ;
	G::Strings m_to_local ;
	G::Strings m_to_remote ;
	std::auto_ptr<std::ostream> m_content ;
	G::Path m_content_path ;
	bool m_eight_bit ;
private:
	bool saveEnvelope( std::ostream & stream , const std::string & where ) const ;
	std::string crlf() const ;
	static bool isEightBit( const std::string & line ) ;
	void deliver( const G::Strings & , const G::Path & , const G::Path & , const G::Path & ) ;
} ;

// ===

// Class: MessageStoreIteratorImp
// Description: A 'body' class for the MessageStoreIterator
// 'handle'. The handle/body pattern allows us to copy
// iterators by value, and therefore return them
// from MessageStore::iterator().
//
class GSmtp::MessageStoreIteratorImp
{
public:
	explicit MessageStoreIteratorImp( const G::Directory & dir ) ;
	std::auto_ptr<GSmtp::StoredMessage> next() ;
public:
	unsigned long m_ref_count ;
private:
	G::DirectoryIterator m_iter ;
private:
	MessageStoreIteratorImp( const MessageStoreIteratorImp & ) ;
	void operator=( const MessageStoreIteratorImp & ) ;
} ;

// ===

GSmtp::MessageStoreIteratorImp::MessageStoreIteratorImp( const G::Directory & dir ) :
	m_iter( dir , "*.envelope" ) ,
	m_ref_count(1UL)
{
}

std::auto_ptr<GSmtp::StoredMessage> GSmtp::MessageStoreIteratorImp::next()
{
	while( !m_iter.error() && m_iter.more() )
	{
		std::auto_ptr<StoredMessageImp> m( new StoredMessageImp(m_iter.filePath()) ) ;
		if( m->lock() )
		{
			std::string reason ;
			if( m->readEnvelope(reason) && m->openContent(reason) )
				return std::auto_ptr<StoredMessage>( m.release() ) ;

			m->fail( reason ) ;
		}
		G_WARNING( "GSmtp::MessageStore: cannot process file: \"" << m_iter.filePath() << "\"" ) ;
	}
	return std::auto_ptr<StoredMessage>(NULL) ;
}

// ===

GSmtp::MessageStore::Iterator::Iterator() :
	m_imp(NULL)
{
}

GSmtp::MessageStore::Iterator::Iterator( MessageStoreIteratorImp * imp ) :
	m_imp(imp)
{
	G_ASSERT( m_imp->m_ref_count == 1UL ) ;
}

std::auto_ptr<GSmtp::StoredMessage> GSmtp::MessageStore::Iterator::next()
{
	return m_imp ? m_imp->next() : std::auto_ptr<StoredMessage>(NULL) ;
}

GSmtp::MessageStore::Iterator::~Iterator()
{
	if( m_imp )
	{
		m_imp->m_ref_count-- ;
		if( m_imp->m_ref_count == 0UL )
			delete m_imp ;
	}
}

GSmtp::MessageStore::Iterator::Iterator( const Iterator & other ) :
	m_imp(other.m_imp)
{
	if( m_imp )
		m_imp->m_ref_count++ ;
}

GSmtp::MessageStore::Iterator & GSmtp::MessageStore::Iterator::operator=( const Iterator & rhs )
{
	if( this != &rhs )
	{
		if( m_imp )
		{
			m_imp->m_ref_count-- ;
			if( m_imp->m_ref_count == 0UL )
				delete m_imp ;
		}
		m_imp = rhs.m_imp ;
		if( m_imp )
		{
			m_imp->m_ref_count++ ;
		}
	}
	return * this ;
}

// ===

GSmtp::StoredMessage::~StoredMessage()
{
	// empty
}

// ===

GSmtp::NewMessage::~NewMessage()
{
	// empty
}

// ===

GSmtp::MessageStore * GSmtp::MessageStore::m_this = NULL ;

GSmtp::MessageStore::MessageStore( const G::Path & directory_path ) :
	m_imp(NULL)
{
	if( m_this == NULL )
		m_this = this ;

	MessageStoreImp::checkPath( directory_path ) ;
	m_imp = new MessageStoreImp( directory_path ) ;
}

GSmtp::MessageStore & GSmtp::MessageStore::instance()
{
	if( m_this == NULL )
		throw NoInstance() ;
	return * m_this ;
}

GSmtp::MessageStore::~MessageStore()
{
	if( m_this == this )
		m_this = NULL ;
	delete m_imp ;
}

G::Path GSmtp::MessageStore::directory() const
{
	return m_imp->dir() ;
}

std::auto_ptr<GSmtp::NewMessage> GSmtp::MessageStore::newMessage( const std::string & from )
{
	std::auto_ptr<NewMessage> new_message( new NewMessageImp(from,*m_imp) ) ;
	return new_message ;
}

bool GSmtp::MessageStore::empty() const
{
	return m_imp->empty() ;
}

std::auto_ptr<GSmtp::StoredMessage> GSmtp::MessageStore::get()
{
	return m_imp->get() ;
}

GSmtp::MessageStore::Iterator GSmtp::MessageStore::iterator()
{
	return m_imp->iterator() ;
}

// ===

GSmtp::NewMessageImp::NewMessageImp( const std::string & from , MessageStoreImp & store ) :
	m_from(from) ,
	m_store(store),
	m_eight_bit(false)
{
	m_seq = store.newSeq() ;

	m_content_path = m_store.contentPath( m_seq ) ;
	G_LOG( "GSmtp::NewMessage: content file: " << m_content_path ) ;

	std::auto_ptr<std::ostream> content_stream = m_store.stream( m_content_path ) ;
	m_content = content_stream ;
}

GSmtp::NewMessageImp::~NewMessageImp()
{
}

void GSmtp::NewMessageImp::addTo( const std::string & to , bool local )
{
	if( local )
		m_to_local.push_back( to ) ;
	else
		m_to_remote.push_back( to ) ;
}

void GSmtp::NewMessageImp::addText( const std::string & line )
{
	if( ! m_eight_bit )
		m_eight_bit = isEightBit(line) ;

	*(m_content.get()) << line << crlf() ;
}

bool GSmtp::NewMessageImp::isEightBit( const std::string & line )
{
	const size_t n = line.length() ;
	for( size_t i = 0U ; i < n ; --i )
	{
		const unsigned char c = static_cast<unsigned char>(line.at(i)) ;
		if( c > 0x7fU )
			return true ;
	}
	return false ;
}

void GSmtp::NewMessageImp::store()
{
	// flush the content file
	m_content->flush() ;
	if( ! m_content->good() )
		throw GSmtp::MessageStore::WriteError( m_content_path.str() ) ;
	m_content <<= 0 ;

	// write the envelope
	G::Path p0 = m_store.envelopeWorkingPath( m_seq ) ;
	G::Path p1 = m_store.envelopePath( m_seq ) ;
	bool ok = false ;
	{
		std::auto_ptr<std::ostream> envelope_stream = m_store.stream( p0 ) ;
		ok = saveEnvelope( *(envelope_stream.get()) , p0.str() ) ;
	}

	// deliver to local mailboxes (in theory)
	if( ok && m_to_local.size() != 0U )
	{
		deliver( m_to_local , m_content_path , p0 , p1 ) ;
	}

	// commit the envelope, or rollback the content
	if( ! ok || ! G::File::rename(p0,p1,G::File::NoThrow() ) )
	{
		G_ASSERT( m_content_path.str().length() != 0U ) ;
		G::File::remove( m_content_path , G::File::NoThrow() ) ;
		throw GSmtp::MessageStore::WriteError( p0.str() ) ;
	}
}

void GSmtp::NewMessageImp::deliver( const G::Strings & to ,
	const G::Path & content_path , const G::Path & envelope_path_now ,
	const G::Path & envelope_path_later )
{
	// could shell out to "procmail" or "deliver" here, but keep it
	// simple and within the scope of a "message-store" class

	G_LOG( "GSmtp::NewMessage: copying message for local recipient(s): "
		<< content_path.basename() << ".local" ) ;

	G::File::copy( content_path.str() , content_path.str()+".local" ) ;
	G::File::copy( envelope_path_now.str() , envelope_path_later.str()+".local" ) ;
}

bool GSmtp::NewMessageImp::saveEnvelope( std::ostream & stream , const std::string & where ) const
{
	G_LOG( "GSmtp::NewMessage: envelope file: " << where ) ;

	const std::string x( MessageStoreImp::x() ) ;

	stream << x << "Format: " << MessageStoreImp::format() << crlf() ;
	stream << x << "Content: " << (m_eight_bit?"8":"7") << "bit" << crlf() ;
	stream << x << "From: " << m_from << crlf() ;
	stream << x << "ToCount: " << (m_to_local.size()+m_to_remote.size()) << crlf() ;
	{
		G::Strings::const_iterator to_p = m_to_local.begin() ;
		for( ; to_p != m_to_local.end() ; ++to_p )
			stream << x << "To-Local: " << *to_p << crlf() ;
	}
	{
		G::Strings::const_iterator to_p = m_to_remote.begin() ;
		for( ; to_p != m_to_remote.end() ; ++to_p )
			stream << x << "To-Remote: " << *to_p << crlf() ;
	}
	stream << x << "End: 1" << crlf() ;
	stream.flush() ;
	return stream.good() ;
}

std::string GSmtp::NewMessageImp::crlf() const
{
	return std::string( "\015\012" ) ;
}

// ===

GSmtp::MessageStoreImp::MessageStoreImp( const G::Path & dir ) :
	m_dir(dir),
	m_seq(1UL)
{
}

//static
std::string GSmtp::MessageStoreImp::x()
{
	return "X-MailRelay-" ;
}

//static
std::string GSmtp::MessageStoreImp::format()
{
	return "#2821.2" ;
}

const G::Path & GSmtp::MessageStoreImp::dir() const
{
	return m_dir ;
}

//static
void GSmtp::MessageStoreImp::checkPath( const G::Path & directory_path )
{
	// (void) G::File::mkdir( directory_path ) ;
	G::Directory dir_test( directory_path ) ;
	if( ! dir_test.valid() )
	{
		throw MessageStore::InvalidDirectory( directory_path.str() ) ;
	}
	if( ! dir_test.valid(true) )
	{
		G_WARNING( "GSmtp::MessageStore: "
			<< "directory not writable: \""
			<< directory_path << "\"" ) ;
	}
}

std::auto_ptr<std::ostream> GSmtp::MessageStoreImp::stream( const G::Path & path )
{
	std::auto_ptr<std::ostream> ptr(
		new std::ofstream( path.pathCstr() ,
			std::ios_base::binary | std::ios_base::out | std::ios_base::trunc ) ) ;
	return ptr ;
}

G::Path GSmtp::MessageStoreImp::contentPath( unsigned long seq ) const
{
	return fullPath( filePrefix(seq) + ".content" ) ;
}

G::Path GSmtp::MessageStoreImp::envelopePath( unsigned long seq ) const
{
	return fullPath( filePrefix(seq) + ".envelope" ) ;
}

G::Path GSmtp::MessageStoreImp::envelopeWorkingPath( unsigned long seq ) const
{
	return fullPath( filePrefix(seq) + ".envelope.new" ) ;
}

std::string GSmtp::MessageStoreImp::filePrefix( unsigned long seq ) const
{
	std::stringstream ss ;
	ss << "emailrelay." << G::Pid() << "." << seq ;
	return ss.str() ;
}

G::Path GSmtp::MessageStoreImp::fullPath( const std::string & filename ) const
{
	G::Path p( m_dir ) ;
	p.pathAppend( filename ) ;
	return p ;
}

unsigned long GSmtp::MessageStoreImp::newSeq()
{
	return m_seq++ ;
}

bool GSmtp::MessageStoreImp::empty() const
{
	G::Directory dir(m_dir) ;
	G::DirectoryIterator iter( dir , "*.envelope" ) ;
	bool no_more = iter.error() || !iter.more() ;
	if( no_more )
		G_DEBUG( "GSmtp::MessageStoreImp: no files to process" ) ;
	return no_more ;
}

GSmtp::MessageStore::Iterator GSmtp::MessageStoreImp::iterator()
{
	return MessageStore::Iterator( new MessageStoreIteratorImp(G::Directory(m_dir)) ) ;
}

std::auto_ptr<GSmtp::StoredMessage> GSmtp::MessageStoreImp::get()
{
	MessageStore::Iterator iter = iterator() ;
	return iter.next() ;
}

// ===

GSmtp::StoredMessageImp::StoredMessageImp( const G::Path & path ) :
	m_envelope_path(path)
{
	G_DEBUG( "StoredMessageImp: \"" << path << "\"" ) ;
}

GSmtp::StoredMessageImp::~StoredMessageImp()
{
}

bool GSmtp::StoredMessageImp::eightBit() const
{
	return m_eight_bit ;
}

bool GSmtp::StoredMessageImp::readEnvelope( std::string & reason )
{
	try
	{
		return readEnvelopeCore( reason ) ;
	}
	catch( std::exception & e )
	{
		reason = e.what() ;
		return false ;
	}
}

bool GSmtp::StoredMessageImp::readEnvelopeCore( std::string & reason )
{
	std::ifstream stream( m_envelope_path.str().c_str() , std::ios_base::binary | std::ios_base::in ) ;
	if( ! stream.good() )
		throw OpenError() ;

	std::string format_line = getline(stream) ;
	if( value(format_line) != MessageStoreImp::format() )
		throw InvalidFormat( value(format_line) + "!=" + MessageStoreImp::format() ) ;

	std::string content_line = getline(stream) ;
	m_eight_bit = value(content_line) == "8bit" ;

	m_from = value(getline(stream)) ;
	G_DEBUG( "GSmtp::StoredMessageImp::readEnvelope: from \"" << m_from << "\"" ) ;

	std::string to_count_line = getline(stream) ;
	unsigned int to_count = G::Str::toUInt( value(to_count_line) ) ;

	for( unsigned int i = 0U ; i < to_count ; i++ )
	{
		std::string to_line = getline(stream) ;
		bool is_local = to_line.find(MessageStoreImp::x()+"To-Local") == 0U ;
		bool is_remote = to_line.find(MessageStoreImp::x()+"To-Remote") == 0U ;
		if( ! is_local && ! is_remote )
			throw InvalidTo(to_line) ;
			
		G_DEBUG( "GSmtp::StoredMessageImp::readEnvelope: to "
			"[" << (i+1U) << "/" << to_count << "] "
			"(" << (is_local?"local":"remote") << ") "
			<< "\"" << value(to_line) << "\"" ) ;

		if( is_local )
			m_to_local.push_back( value(to_line) ) ;
		else
			m_to_remote.push_back( value(to_line) ) ;
	}

	std::string end = getline(stream) ;
	if( end.find(MessageStoreImp::x()+"End") != 0U )
		throw NoEnd() ;

	if( m_to_remote.size() == 0U )
		throw NoRecipients() ;

	if( ! stream.good() )
		throw StreamError() ;

	return stream.good() ;
}

bool GSmtp::StoredMessageImp::openContent( std::string & reason )
{
	try
	{
		G::Path content_path = contentPath() ;
		G_DEBUG( "GSmtp::MessageStoreImp::openContent: \"" << content_path << "\"" ) ;
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
		G_WARNING( "GSmtp::MessageStoreImp: exception: " << e.what() ) ;
		reason = e.what() ;
		return false ;
	}
}

std::string GSmtp::StoredMessageImp::getline( std::istream & stream ) const
{
	return G::Str::readLineFrom( stream , crlf() ) ;
}

std::string GSmtp::StoredMessageImp::value( const std::string & s ) const
{
	size_t pos = s.find(' ') ;
	if( pos == std::string::npos )
		throw MessageStore::FormatError() ;
	return s.substr(pos+1U) ;
}

std::string GSmtp::StoredMessageImp::crlf() const
{
	return std::string( "\015\012" ) ;
}

bool GSmtp::StoredMessageImp::lock()
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

void GSmtp::StoredMessageImp::fail( const std::string & reason )
{
	try
	{
		{
			std::ofstream file( m_envelope_path.str().c_str() ,
				std::ios_base::binary | std::ios_base::ate ) ;
			file << MessageStoreImp::x() << "Reason: " << reason ;
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

void GSmtp::StoredMessageImp::destroy()
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

const std::string & GSmtp::StoredMessageImp::from() const
{
	return m_from ;
}

const G::Strings & GSmtp::StoredMessageImp::to() const
{
	return m_to_remote ;
}

std::auto_ptr<std::istream> GSmtp::StoredMessageImp::extractContentStream()
{
	G_ASSERT( m_content.get() != NULL ) ;
	return m_content ;
}

G::Path GSmtp::StoredMessageImp::contentPath() const
{
	G::Path path( m_envelope_path ) ; // "foo.envelope.busy"
	path.removeExtension() ; // "foo.envelope"
	path.setExtension( "content" ) ; // "foo.content"
	return path ;
}

