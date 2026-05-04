//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
// 
// Copyright (c) 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
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
/// \file goptionreader.cpp
///

#include "gdef.h"
#include "goptionreader.h"
#include "gdirectory.h"
#include "gfile.h"
#include "gstr.h"
#include "gstringview.h"
#include "gstringtoken.h"
#include "gstringfield.h"
#include "glog.h"
#include <algorithm>
#include <vector>
#include <fstream>

void G::OptionReader::read( StringArray & options_out , PathArray & files_out ,
	StringArray & errors_out , const Path & path , const Config & config )
{
	read( options_out , &files_out , &errors_out , path , config ) ;
}

void G::OptionReader::read( StringArray & options_out ,
	const Path & path , const Config & config )
{
	read( options_out , nullptr , nullptr , path , config ) ;
}

void G::OptionReader::read( StringArray & options_out , PathArray * files_out_p ,
	StringArray * errors_out_p , const Path & path , const Config & config )
{
	PathArray files ;
	findFiles( files , path , config ) ;
	if( files.empty() && !path.empty() && !config.uapi() )
	{
		fail( errors_out_p , path ) ;
		return ;
	}

	if( config.debug )
	{
		for( const auto & file : files )
			std::cerr << "config-file: " << file << "\n" ;
	}

	for( const Path & file : files )
	{
		if( !add( options_out , file , config.limit ) )
			fail( errors_out_p , file ) ;
	}

	if( files_out_p )
		files_out_p->swap( files ) ;
}

void G::OptionReader::fail( StringArray * errors_out_p , const Path & path )
{
	if( errors_out_p )
		errors_out_p->push_back( FileError(path.str()).what() ) ;
	else
		throw FileError( path.str() ) ;
}

void G::OptionReader::findFiles( std::vector<Path> & out ,
	const Path & path , const Config & config )
{
	if( config.mode == Mode::single && !path.empty() )
	{
		out.push_back( path ) ;
	}
	else if( config.mode == Mode::dropins && !path.empty() )
	{
		out = findDropins( path.str()+config.suffix , path.extension() ) ;
		if( exists(path) )
			out.insert( out.begin() , path ) ;
	}
	else if( config.mode == Mode::uapi )
	{
		for( const auto & dir : config.dirs )
		{
			auto basepath = dir/config.filename ;
			bool seen_base = seen( out , basepath ) ;
			if( !seen_base && exists(basepath) )
				out.push_back( basepath ) ;

			auto dropins = findDropins( dir/(config.filename+config.suffix) , Path(config.filename).extension() ) ;
			for( const auto & dropin : dropins )
			{
				bool seen_dropin = seen( out , dropin ) ;
				if( !seen_dropin )
					out.push_back( dropin ) ;
			}
		}
	}
}

bool G::OptionReader::exists( const Path & path )
{
	auto stat = File::stat( path ) ;
	return stat.error == 0 && !stat.is_dir ;
}

bool G::OptionReader::seen( const std::vector<Path> & list , const Path & file )
{
	return list.end() != std::find_if( list.begin() , list.end() ,
		[&file](const Path &p_){return file.basename()==p_.basename();} ) ;
}

bool G::OptionReader::add( StringArray & out , const Path & path , std::size_t limit )
{
	std::ifstream f ;
	File::open( f , path ) ; // (no G::Root)
	if( !f.good() )
		return false ;

	std::string line ;
	std::size_t n = 0U ;
	while( Str::readLine(f,line) && ( limit == 0U || n < limit ) )
	{
		if( line.find('\0') != std::string::npos )
			throw G::Exception( "invalid character in configuration file" , path.str() ) ;

		Str::trimRight( line , "\r" ) ;
		std::string_view sv( line ) ;
		StringTokenView t( sv , " =\t" , 3U ) ;
		std::string_view key = t() ;
		if( key.empty() || key.find('#') == 0U )
			continue ;

		std::string_view value = (++t)() ;
		if( !value.empty() )
			value = Str::trimRightView( sv.substr(t.pos()) , " \t" ) ;
		if( value.size() >= 2U && value[0] == '"' && value[value.size()-1U] == '"' )
			value = value.substr( 1U , value.size() - 2U ) ;

		out.push_back( std::string(2U,'-')
			.append(key.data(),key.size())
			.append(value.empty()?0U:1U,'=')
			.append(value.data(),value.size()) ) ;

		n++ ;
	}
	if( limit && n == limit )
		throw G::Exception( "too many lines in configuration file" , path.str() ) ;
	return true ;
}

std::vector<G::Path> G::OptionReader::findDropins( const Path & dir , const std::string & extension )
{
	std::vector<DirectoryList::Item> list ;
	DirectoryList::readType( dir , extension , list ) ;
	std::vector<Path> result ;
	result.reserve( list.size() ) ;
	for( const auto & item : list )
	{
		if( !item.m_is_dir )
			result.push_back( item.m_path ) ;
	}
	return result ;
}

// ==

G::OptionReader::Config::Config() :
	mode(Mode::dropins) ,
	limit(10000U) ,
	suffix(".d") ,
	debug(false)
{
}

bool G::OptionReader::Config::parse( std::string_view spec , std::string_view name )
{
	if( spec == "single" )
	{
		mode = Mode::single ;
		return true ;
	}
	else if( spec.empty() || spec == "dropins" || spec == "dropins:debug" )
	{
		mode = Mode::dropins ;
		debug = spec == "dropins:debug" ;
		return true ;
	}
	else if( spec.find("uapi") == 0U )
	{
		mode = Mode::uapi ;
		std::string name_str = sv_to_string( name ) ;
		filename = name_str + (is_windows()?".cfg":".conf") ;
		dirs.push_back( "/etc/"+name_str ) ;
		dirs.push_back( "/run/"+name_str ) ;
		dirs.push_back( "/usr/lib/"+name_str ) ;

		StringFieldView f( spec , is_windows()?";":":" , 1U ) ;
		++f ;
		if( f.valid() && f() == "debug" )
			debug = true , ++f ;
		if( f.valid() && f.size() != 0U )
			filename = f() ;
		++f ;
		for( bool first = true ; f ; ++f , first=false )
		{
			if( f.size() == 0U )
				continue ;
			if( first )
				dirs.clear() ;
			dirs.push_back( f() ) ;
		}
		return true ;
	}
	else
	{
		return false ;
	}
}

