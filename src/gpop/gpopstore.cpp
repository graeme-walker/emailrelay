//
// Copyright (C) 2001-2023 Graeme Walker <graeme_walker@users.sourceforge.net>
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
/// \file gpopstore.cpp
///

#include "gdef.h"
#include "gpopstore.h"
#include "gstr.h"
#include "gfile.h"
#include "gdirectory.h"
#include "gtest.h"
#include "groot.h"
#include "gassert.h"
#include <algorithm>
#include <iterator>
#include <sstream>
#include <fstream>

namespace GPop
{
	namespace StoreImp /// An implementation namespace for GPop::Store
	{
		struct FileReader /// Used by GPop::Store like G::Root when reading.
		{
			FileReader() ;
		} ;
		struct DirectoryReader : private G::Root /// Used by GPop::Store like G::Root when reading a directory.
		{
		} ;
		struct FileDeleter : private G::Root /// Used by GPop::Store like G::Root when deleting.
		{
		} ;
		unsigned long toSize( const std::string & s )
		{
			return (s.empty() || !G::Str::isULong(s) ) ? 0UL : G::Str::toULong(s,G::Str::Limited()) ;
		}
	}
}

// ==

GPop::Store::Store( const G::Path & path , bool by_name , bool allow_delete ) :
	m_path(path) ,
	m_by_name(by_name) ,
	m_allow_delete(allow_delete)
{
	if( !accessible( path , by_name?false:allow_delete ) )
		throw InvalidDirectory() ;

	if( by_name )
	{
		// read the spool directory
		G::DirectoryList iter ;
		{
			StoreImp::DirectoryReader claim_reader ;
			iter.readAll( path ) ;
		}

		// check every sub-directory is accessible() and count them
		int n = 0 ;
		while( iter.more() )
		{
			if( iter.isDir() )
			{
				bool ok = accessible( iter.filePath() , allow_delete ) ;
				G_DEBUG( "GPop::Store::ctor: [" << iter.filePath() << "]: " << (ok?"good":"bad") ) ;
				if( ok )
					n++ ;
			}
		}

		// warn if no accessible() sub-directories
		if( n == 0 )
		{
			G_WARNING( "GPop::Store::ctor: no sub-directories for pop-by-name found in \"" << path << "\": "
				<< "create one sub-directory for each authorised pop account" ) ;
		}
	}
}

bool GPop::Store::accessible( const G::Path & dir_path , bool for_write )
{
	G::Directory dir_test( dir_path ) ;
	bool ok = false ;
	if( for_write )
	{
		std::string tmp_filename = G::Directory::tmp() ;
		StoreImp::FileDeleter claim_deleter ;
		ok = dir_test.valid() && dir_test.writeable( tmp_filename ) ;
	}
	else
	{
		StoreImp::FileReader claim_reader ;
		ok = dir_test.valid() ;
	}
	if( !ok )
	{
		const char * op = for_write ? "writing" : "reading" ;
		G_WARNING( "GPop::Store: directory not valid for " << op << ": \"" << dir_path << "\"" ) ;
	}
	return ok ;
}

G::Path GPop::Store::dir() const
{
	return m_path ;
}

bool GPop::Store::allowDelete() const
{
	return m_allow_delete ;
}

bool GPop::Store::byName() const
{
	return m_by_name ;
}

// ===

GPop::StoreMessage::StoreMessage( const std::string & name_in , Size size_in , bool in_parent_in ) :
	name(name_in) ,
	size(size_in) ,
	in_parent(in_parent_in) ,
	deleted(false)
{
}

G::Path GPop::StoreMessage::cpath( const G::Path & edir , const G::Path & sdir ) const
{
	return in_parent ? cpath(sdir) : cpath(edir) ;
}

G::Path GPop::StoreMessage::cpath( const G::Path & dir ) const
{
	return G::Path( dir , name+".content" ) ;
}

G::Path GPop::StoreMessage::epath( const G::Path & edir ) const
{
	return G::Path( edir , name+".envelope" ) ;
}

GPop::StoreMessage GPop::StoreMessage::invalid()
{
	return {{},0,false} ;
}

std::string GPop::StoreMessage::uidl() const
{
	return name + ".content" ;
}

// ===

GPop::StoreUser::StoreUser( Store & store , const std::string & user ) :
	m_store(store) ,
	m_user(user) ,
	m_edir(store.byName()?G::Path(store.dir(),user):store.dir()) ,
	m_sdir(store.dir())
{
	G_ASSERT( !user.empty() ) ;

	// build a list of envelope files, with content file sizes
	{
		StoreImp::DirectoryReader claim_reader ;
		G::DirectoryList iter ;
		std::size_t n = iter.readType( m_edir , ".envelope" ) ;
		m_list.reserve( n ) ;
		while( iter.more() )
		{
			std::string ename = iter.fileName() ;
			std::string name = G::Str::head( ename , ename.rfind('.') ) ;
			std::string cname = name + ".content" ;

			bool in_parent =
				!G::File::exists( G::Path(m_edir,cname) , std::nothrow ) &&
				m_store.byName() &&
				G::File::exists( G::Path(m_sdir,cname) , std::nothrow ) ;

			G::Path cpath = in_parent ? G::Path(m_sdir,cname) : G::Path(m_edir,cname) ;

			auto csize = StoreImp::toSize( G::File::sizeString(cpath) ) ;
			if( csize )
				m_list.emplace_back( name , csize , in_parent ) ;
		}
	}
}

// ==

GPop::StoreList::StoreList()
= default ;

GPop::StoreList::StoreList( const StoreUser & store_user , bool allow_delete ) :
	m_allow_delete(allow_delete) ,
	m_edir(store_user.m_edir) ,
	m_sdir(store_user.m_sdir) ,
	m_list(store_user.m_list)
{
}

GPop::StoreList::List::const_iterator GPop::StoreList::cbegin() const
{
	return m_list.cbegin() ;
}

GPop::StoreList::List::const_iterator GPop::StoreList::cend() const
{
	return m_list.cend() ;
}

GPop::StoreMessage::Size GPop::StoreList::messageCount() const
{
	std::size_t n = std::count_if( m_list.begin() , m_list.end() ,
		[](const StoreMessage &msg_){return !msg_.deleted;} ) ;

	return static_cast<StoreMessage::Size>(n) ;
}

GPop::StoreList::Size GPop::StoreList::totalByteCount() const
{
	Size total = 0 ;
	for( const auto & item : m_list )
	{
		if( !item.deleted )
			total += item.size ;
	}
	return total ;
}

bool GPop::StoreList::valid( int id ) const
{
	return id >= 1 && id <= static_cast<int>(m_list.size()) && !m_list.at(id-1).deleted ;
}

GPop::StoreMessage GPop::StoreList::get( int id ) const
{
	G_ASSERT( valid(id) ) ;
	return valid(id) ? m_list.at(id-1) : StoreMessage::invalid() ;
}

GPop::StoreList::Size GPop::StoreList::byteCount( int id ) const
{
	G_ASSERT( valid(id) ) ;
	return valid(id) ? m_list.at(id-1).size : Size(0) ;
}

std::unique_ptr<std::istream> GPop::StoreList::content( int id ) const
{
	G_ASSERT( valid(id) ) ;
	if( !valid(id) ) throw CannotRead( std::to_string(id) ) ;

	G::Path cpath = m_list.at(id-1).cpath(m_edir,m_sdir) ;
	G_DEBUG( "GPop::StoreList::get: " << id << " " << cpath ) ;

	auto fstream = std::make_unique<std::ifstream>() ;
	{
		StoreImp::FileReader claim_reader ;
		G::File::open( *fstream , cpath ) ;
	}

	if( !fstream->good() )
		throw CannotRead( cpath.str() ) ;

	return fstream ;
}

void GPop::StoreList::remove( int id )
{
	if( valid(id) )
		m_list.at(id-1).deleted = true ;
}

void GPop::StoreList::commit()
{
	bool all_ok = true ;
	for( const auto & item : m_list )
	{
		if( item.deleted && m_allow_delete )
		{
			deleteFile( item.epath(m_edir) , all_ok ) ;
			if( !shared(item) ) // race condition could leave content files undeleted
				deleteFile( item.cpath(m_edir,m_sdir) , all_ok ) ;
		}
		else if( item.deleted )
		{
			G_DEBUG( "StoreList::doCommit: not deleting \"" << item.name << "\"" ) ;
		}
	}
	if( ! all_ok )
		throw CannotDelete() ;
}

void GPop::StoreList::deleteFile( const G::Path & path , bool & all_ok ) const
{
	bool ok = false ;
	{
		StoreImp::FileDeleter claim_deleter ;
		ok = G::File::remove( path , std::nothrow ) ;
	}
	all_ok = ok && all_ok ;
	if( ! ok )
		G_ERROR( "StoreList::deleteFile: failed to delete " << path ) ;
}

#ifndef G_LIB_SMALL
std::string GPop::StoreList::uidl( int id ) const
{
	G_ASSERT( valid(id) ) ;
	return valid(id) ? m_list.at(id-1).uidl() : std::string() ;
}
#endif

void GPop::StoreList::rollback()
{
	for( auto & item : m_list )
		item.deleted = false ;
}

bool GPop::StoreList::shared( const StoreMessage & message ) const
{
	if( !message.in_parent )
	{
		return false ;
	}
	else
	{
		// look for envelopes that share this content
		G_DEBUG( "GPop::StoreList::shared: test sharing of " << message.cpath(m_edir,m_sdir) ) ;

		// start with the main spool directory
		bool found = G::File::exists( message.epath(m_sdir) , std::nothrow ) ;
		G_DEBUG_IF( found , "GPop::StoreList::shared: content shared: envelope: " << message.epath(m_sdir) ) ;

		// and then sub-directories
		G::DirectoryList iter ;
		{
			StoreImp::DirectoryReader claim_reader ;
			iter.readAll( m_sdir ) ;
		}
		while( iter.more() && !found )
		{
			if( !iter.isDir() )
				continue ;

			G::Path sub_dir = iter.filePath() ;
			G_DEBUG( "GPop::StoreList::shared: checking sub-directory: " << sub_dir ) ;

			G::Path epath( message.epath(m_sdir+sub_dir.basename()) ) ;
			found = G::File::exists( epath , std::nothrow ) ;
			G_DEBUG_IF( found , "GPop::StoreList::shared: content shared: envelope: " << epath ) ;
		}
		G_DEBUG_IF( !found , "GPop::StoreList::shared: content not shared: no matching envelope" ) ;
		return found ;
	}
	return true ;
}

GPop::StoreImp::FileReader::FileReader() // NOLINT modernize-use-equals-default because of -Wunused
{
}

