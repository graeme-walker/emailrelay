//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gcopyfilter.cpp
///

#include "gdef.h"
#include "gcopyfilter.h"
#include "gdirectory.h"
#include "gstringtoken.h"
#include "groot.h"
#include "gfile.h"
#include "glog.h"

GFilters::CopyFilter::CopyFilter( GNet::EventState es , GStore::FileStore & store ,
	Filter::Type filter_type , const Filter::Config & filter_config , const std::string & spec ) :
		SimpleFilterBase(es,filter_type,"copy:") ,
		m_store(store) ,
		m_filter_config(filter_config) ,
		m_spec(spec)
{
	std::string_view spec_sv = spec ;
	for( G::StringTokenView t( spec_sv , ";" , 1U ) ; t ; ++t )
	{
		if( t() == "p" || t() == "pop" ) m_delivery_config.pop_by_name = true ;
		if( t() == "h" || t() == "hardlink" ) m_delivery_config.hardlink = true ;
		if( t() == "n" || t() == "nodelete" || t() == "no_delete" ) m_delivery_config.no_delete = true ;
		if( t() == "s" || t() == "simple" ) m_delivery_config.simple = true ;
	}
}

GSmtp::Filter::Result GFilters::CopyFilter::run( const GStore::MessageId & message_id ,
	bool & , GStore::FileStore::State e_state )
{
	G::Path content_path = m_store.contentPath( message_id ) ;
	G::Path envelope_path = m_store.envelopePath( message_id , e_state ) ;
	GStore::Envelope envelope = GStore::FileStore::readEnvelope( envelope_path ) ;

	G::DirectoryList list ;
	{
		G::Root claim_root ;
		list.readDirectories( m_store.directory() ) ;
	}

	G::StringArray copy_names ;
	G::StringArray ignore_names ;
	while( list.more() )
	{
		G::Path subdir = list.filePath() ;
		std::string name = subdir.basename() ;
		if( name.empty() || name.at(0U) == '.' || name == "postmaster" )
		{
			ignore_names.push_back( name ) ;
		}
		else
		{
			copy_names.push_back( name ) ;
			GStore::FileDelivery::deliverTo( m_store , "copy" ,
				subdir , envelope_path , content_path ,
				m_delivery_config ) ;
		}
	}

	if( copy_names.empty() )
	{
		G_WARNING_ONCE( "GFilters::CopyFilter::start: copy filter: "
			"no sub-directories of [" << m_store.directory() << "] to copy in to" ) ;
		return Result::ok ;
	}
	else
	{
		G_LOG( "GFilters::CopyFilter::start: " << prefix() << ": "
			<< message_id.str() << " copied to [" << G::Str::join(",",copy_names) << "]"
			<< (ignore_names.empty()?"":" not [") << G::Str::join(",",ignore_names)
			<< (ignore_names.empty()?"":"]") ) ;

		if( m_delivery_config.no_delete )
		{
			return Result::ok ;
		}
		else
		{
			G::Root claim_root ;
			G::File::remove( envelope_path ) ;
			if( !m_delivery_config.pop_by_name )
				G::File::remove( content_path ) ;
			return Result::abandon ;
		}
	}
}

