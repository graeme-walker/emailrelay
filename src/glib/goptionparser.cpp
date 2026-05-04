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
/// \file goptionparser.cpp
///

#include "gdef.h"
#include "goptionparser.h"
#include "gformat.h"
#include "ggettext.h"
#include "gstr.h"
#include "gassert.h"
#include "glog.h"
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace G
{
	class OptionParserImp ;
}

class G::OptionParserImp
{
public:
	using CallbackFn = OptionParser::CallbackFn ;
	using Config = OptionParser::Config ;
	using StringPair = std::pair<std::string,std::string> ;

	OptionParserImp( OptionMap & values_out , StringArray & errors_out , StringArray & args_out , Config ) ;
	void parseImp( const StringArray & , const Options & , CallbackFn ) ;
	void parseOneName( const StringArray & names , std::size_t i , const StringArray & , const Options & ) ;
	StringArray parseNames( const StringArray & , const Options & , std::string_view ) ;
	bool parseArgs( const StringArray & , const Options & ) ;
	static std::string tooManyArguments() ;

private:
	std::string getKey( const std::string & key , CallbackFn callback_fn , bool & discard ) ;
	static std::pair<bool,std::string> argn( const G::StringArray & , std::size_t i ) ;
	static std::string::size_type eqPos( const std::string & ) ;
	static std::string eqValue( const std::string & , std::string::size_type ) ;
	void processOption( const Options & , char , const std::string & , bool ) ;
	void processOption( const Options & , char , const std::string & , const std::string & value ) ;
	void processOption( const Options & , char , const std::string & , const std::pair<bool,std::string> & ) ;
	void processOptionValue( const Options & , char , const std::string & name , std::string_view value , bool ) ;
	void errorNoValue( char , const std::string & ) ;
	void errorUnknownOption( char , const std::string & ) ;
	void errorDubiousValue( char , const std::string & , const std::string & ) ;
	void errorDuplicate( char , const std::string & ) ;
	void errorExtraValue( char , const std::string & , const std::string & ) ;
	void errorConflict( char , const std::string & ) ;
	void error( const std::string & ) ;
	void insert( OptionMap & , const std::string & , const OptionValue & ) ;
	void insert( OptionMap & , const std::string & , const G::StringArray & ) ;
	static bool isOldOption( const std::string & ) ;
	static bool isNewOption( const std::string & ) ;
	static bool isAnOptionSet( const std::string & ) ;
	static std::string qname( char c , const std::string & name ) ;
	static const Option * find( const G::Options & , const std::string & , std::string * = nullptr ) ;

private:
	OptionMap & m_map ;
	StringArray & m_errors ;
	StringArray & m_args_out ;
	Config m_config ;
	bool m_discard {false} ; // implicit parameter to processOption()
} ;

G::StringArray G::OptionParser::parse( const StringArray & args_in , const Options & spec ,
	OptionMap & values_out , StringArray & errors_out , Config config ,
	CallbackFn callback_fn )
{
	G::StringArray args_out ;
	OptionParserImp imp( values_out , errors_out , args_out , config ) ;
	imp.parseImp( args_in , spec , callback_fn ) ;
	return args_out ;
}

G::StringArray G::OptionParser::parse( const StringArray & args_in , const Options & spec ,
	const StringArray & names , Maps & maps_out ,
	StringArray & errors_out , Config config )
{
	if( names.empty() )
		return {} ;
	StringArray args_out ;
	for( std::size_t i = 0U ; i < names.size() ; i++ )
	{
		maps_out.emplace_back( names[i] , OptionMap() ) ;
		OptionParserImp imp( maps_out.back().second , errors_out , args_out , config ) ;
		imp.parseOneName( names , i , args_in , spec ) ;
	}
	return args_out ;
}

G::StringArray G::OptionParser::names( const StringArray & args_in , const Options & spec ,
	StringArray & errors_out , Config config , std::string_view introducer )
{
	if( introducer.empty() )
		return {{}} ;
	OptionMap map ;
	StringArray args_out ;
	OptionParserImp imp( map , errors_out , args_out , config ) ;
	return imp.parseNames( args_in , spec , introducer ) ;
}

G::OptionParser::StringPair G::OptionParser::file( const StringArray & args_in , const Options & spec ,
	StringArray & errors_out , Config config , std::string_view modifier_key )
{
	// do a basic parse -- allow section names but ignore them
	OptionMap map ; // ignored
	StringArray errors_tmp ; // ignored
	StringArray args_out ;
	OptionParserImp imp( map , errors_tmp , args_out , config ) ;
	if( !imp.parseArgs( args_in , spec ) )
		errors_out.push_back( OptionParserImp::tooManyArguments() ) ;

	// extract the command-line option that can be used to
	// modify how the config file is processed
	std::string modifier_value ;
	G_ASSERT( modifier_key.empty() || spec.singlevalued(modifier_key) ) ;
	if( !modifier_key.empty() )
		modifier_value = map.value( modifier_key ) ;

	// take the non-option argument as the config file
	std::string config_file ;
	if( args_out.size() )
		config_file = args_out[0] ;

	return {config_file,modifier_value} ;
}

// ==

G::OptionParserImp::OptionParserImp( OptionMap & values_out , StringArray & errors_out ,
	StringArray & args_out , Config config ) :
		m_map(values_out) ,
		m_errors(errors_out) ,
		m_args_out(args_out) ,
		m_config(config)
{
}

void G::OptionParserImp::parseImp( const StringArray & args_in , const Options & spec ,
	CallbackFn callback_fn )
{
	std::size_t i = m_config.start_position ;
	std::size_t ignorable = m_config.ignore_non_options ;
	for( ; i < args_in.size() ; i++ )
	{
		const std::string & arg = args_in.at(i) ;

		if( arg == "--" ) // end-of-options marker
		{
			i++ ;
			break ;
		}

		if( isAnOptionSet(arg) ) // eg. "-ltv"
		{
			for( std::size_t n = 1U ; n < arg.length() ; n++ )
			{
				char c = arg.at( n ) ;
				std::string key = getKey( spec.lookup(c) , callback_fn , m_discard ) ;
				processOption( spec , c , key , true ) ;
			}
		}
		else if( isOldOption(arg) ) // eg. "-v"
		{
			char c = arg.at(1U) ;
			std::string key = getKey( spec.lookup(c) , callback_fn , m_discard ) ;
			if( spec.unvalued(key) || spec.defaulting(key) )
				processOption( spec , c , key , true ) ;
			else
				processOption( spec , c , key , argn(args_in,++i) ) ;
		}
		else if( isNewOption(arg) ) // eg. "--foo"
		{
			std::string key_value = arg.substr( 2U ) ; // "foo" or "foo=..."
			std::size_t pos_eq = eqPos( key_value ) ;
			bool has_eq = pos_eq != std::string::npos ;
			std::string key_in = has_eq ? key_value.substr(0U,pos_eq) : key_value ; // "foo"
			std::string value = eqValue( key_value , pos_eq ) ; // "..."
			std::string key = getKey( key_in , callback_fn , m_discard ) ;

			if( has_eq )
				processOption( spec , '\0' , key , value ) ;
			else if( spec.unvalued(key) || spec.defaulting(key) )
				processOption( spec , '\0' , key , true ) ;
			else
				processOption( spec , '\0' , key , argn(args_in,++i) ) ;
		}
		else if( ignorable != 0U )
		{
			--ignorable ;
			m_args_out.push_back( arg ) ;
		}
		else
		{
			break ;
		}
	}
	if( !m_config.keep_negatives )
		m_map.clearOffValues() ; // (new)
	for( ; i < args_in.size() ; i++ )
		m_args_out.push_back( args_in.at(i) ) ;
}

bool G::OptionParserImp::parseArgs( const StringArray & args_in , const Options & spec )
{
	// allow section names but shove all options into one map
	parseImp( args_in , spec ,
		[&spec](const std::string & s_,const std::string &){
			const auto * option_p = find( spec , s_ ) ;
			return option_p ? option_p->name() : s_ ;
		} ) ;
	return m_args_out.size() <= 1U ;
}

G::StringArray G::OptionParserImp::parseNames( const StringArray & args_in , const Options & spec ,
	std::string_view introducer )
{
	StringArray names ;
	parseImp( args_in , spec ,
		[introducer,&names,spec](const std::string & s_,const std::string & ignore_){
			std::string prefix ;
			const auto * option_p = find( spec , s_ , &prefix ) ;
			if( option_p && !prefix.empty() && option_p->name() == introducer )
				names.push_back( s_.substr(0U,s_.size()-introducer.size()-1U) ) ;
			return option_p ? (ignore_+option_p->name()) : s_ ;
		} ) ;
	if( !m_errors.empty() )
		return {} ;
	if( names.empty() )
		names.emplace_back() ;
	return names ;
}

void G::OptionParserImp::parseOneName( const StringArray & names , std::size_t i ,
	const StringArray & args_in , const Options & spec )
{
	return parseImp( args_in , spec ,
		[&names,spec,i](const std::string & s_,const std::string & ignore_){
			std::string prefix ;
			auto option_p = OptionParserImp::find( spec , s_ , &prefix ) ;
			if( option_p == nullptr )
				return s_ ;
			else if( s_ == option_p->name() && i == 0U )
				return option_p->name() ; // no prefix, first config
			else if( s_ == option_p->name() )
				return ignore_+option_p->name() ; // no prefix, not first config
			else if( s_ == (names[i]+"-"+option_p->name()) )
				return option_p->name() ; // our prefix
			else if( std::find(names.begin(),names.end(),prefix) != names.end() )
				return ignore_+option_p->name() ; // valid prefix but not ours
			else
				return s_ ; // invalid, fail as normal
		} ) ;
}

const G::Option * G::OptionParserImp::find( const Options & spec ,
	const std::string & parser_name , std::string * prefix_p )
{
	if( parser_name.empty() )
		return nullptr ; // never gets here

	// exact match
	const Option * option_p = spec.find( parser_name ) ;
	if( option_p )
		return option_p ;

	// tail match
	std::string tail = Str::tail( parser_name , "-" ) ;
	if( parser_name[0] == '-' || tail.empty() )
		return nullptr ;
	if( prefix_p )
		*prefix_p = Str::head( parser_name , "-" ) ;
	return spec.find( tail ) ;
}

std::string G::OptionParserImp::getKey( const std::string & key , CallbackFn callback_fn , bool & discard )
{
	// let user's callback modify the key and set a discard flag
	discard = false ;
	if( !key.empty() && callback_fn )
	{
		std::string cb_out = callback_fn( key , std::string(1U,'\0') ) ;
		if( !cb_out.empty() && cb_out.at(0U) == '\0' ) // (was '-')
		{
			discard = true ;
			cb_out = cb_out.substr( 1U ) ;
		}
		return cb_out ;
	}
	else
	{
		return key ;
	}
}

std::pair<bool,std::string> G::OptionParserImp::argn( const G::StringArray & args , std::size_t i )
{
	if( i >= args.size() )
		return {false,{}} ;
	else
		return {true,args.at(i)} ;
}

void G::OptionParserImp::processOption( const Options & spec , char c ,
	const std::string & name , const std::string & value )
{
	if( m_discard )
		;
	else if( !spec.valid(name) )
		errorUnknownOption( c , name ) ;
	else if( spec.unvalued(name) && G::Str::isPositive(value) )
		processOption( spec , c , name , true ) ;
	else if( spec.unvalued(name) && G::Str::isNegative(value) )
		processOption( spec , c , name , false ) ;
	else
		processOptionValue( spec , c , name , value , false ) ;
}

void G::OptionParserImp::processOption( const Options & spec , char c ,
	const std::string & name , const std::pair<bool,std::string> & value_pair )
{
	if( m_discard )
		;
	else if( !spec.valid(name) )
		errorUnknownOption( c , name ) ;
	else if( value_pair.first && spec.unvalued(name) && G::Str::isPositive(value_pair.second) )
		processOptionValue( spec , c , name , G::Str::positive() , false ) ;
	else if( value_pair.first && spec.unvalued(name) && G::Str::isNegative(value_pair.second) )
		processOptionValue( spec , c , name , G::Str::negative() , false ) ;
	else if( value_pair.first )
		processOptionValue( spec , c , name , value_pair.second , true ) ;
	else
		errorNoValue( c , name ) ;
}

void G::OptionParserImp::processOptionValue( const Options & spec , char c ,
	const std::string & name , std::string_view value , bool fail_if_dubious_value )
{
	if( m_discard )
		;
	else if( !value.empty() && value[0] == '-' && fail_if_dubious_value )
		errorDubiousValue( c , name , sv_to_string(value) ) ;
	else if( spec.unvalued(name) )
		errorExtraValue( c , name , sv_to_string(value) ) ;
	else if( m_map.contains(name) && !spec.multivalued(name) && m_map.value(name) != value )
		errorDuplicate( c , name ) ;
	else if( spec.multivalued(name) )
		insert( m_map , name , G::Str::splitIntoFields(value,',') ) ;
	else
		insert( m_map , name , OptionValue(value) ) ;
}

void G::OptionParserImp::processOption( const Options & spec , char c ,
	const std::string & name , bool value )
{
	if( m_discard )
		;
	else if( !spec.valid(name) )
		errorUnknownOption( c , name ) ;
	else if( spec.defaulting(name) && m_map.contains(name) && !m_map.value(name).empty() )
		errorConflict( c , name ) ;
	else if( spec.defaulting(name) )
		insert( m_map , name , OptionValue(std::string()) ) ;
	else if( spec.valued(name) )
		errorNoValue( c , name ) ;
	else if( value && m_map.containsOff(name) )
		errorConflict( c , name ) ;
	else if( !value && m_map.containsOn(name) )
		errorConflict( c , name ) ;
	else
		insert( m_map , name , value ? OptionValue::on() : OptionValue::off() ) ;
}

void G::OptionParserImp::insert( OptionMap & map , const std::string & key , const OptionValue & value )
{
	if( !m_discard )
		map.insert( {key,value} ) ;
}

void G::OptionParserImp::insert( OptionMap & map , const std::string & key , const G::StringArray & values )
{
	for( const std::string & value : values )
		insert( map , key , OptionValue(value) ) ;
}

std::string::size_type G::OptionParserImp::eqPos( const std::string & s )
{
	std::string::size_type p = s.find_first_not_of( "abcdefghijklmnopqrstuvwxyz0123456789-_" ) ;
	return p != std::string::npos && s.at(p) == '=' ? p : std::string::npos ;
}

std::string G::OptionParserImp::eqValue( const std::string & s , std::string::size_type pos )
{
	return (pos+1U) == s.length() ? std::string() : s.substr(pos+1U) ;
}

bool G::OptionParserImp::isOldOption( const std::string & arg )
{
	return
		( arg.length() > 1U && arg.at(0U) == '-' ) &&
		!isNewOption( arg ) ;
}

bool G::OptionParserImp::isNewOption( const std::string & arg )
{
	return arg.length() > 2U && arg.at(0U) == '-' && arg.at(1U) == '-' ;
}

bool G::OptionParserImp::isAnOptionSet( const std::string & arg )
{
	return isOldOption(arg) && arg.length() > 2U ;
}

void G::OptionParserImp::errorDubiousValue( char , const std::string & name , const std::string & value )
{
	std::string s = str(
		format(txt("%1% is probably a mistake, use %2% or %3%")) %
			("\"--"+name+" "+value+"\"") %
			("\"--"+name+"=... "+value+"\"") %
			("\"--"+name+"="+value+"\"") ) ;
	error( s ) ;
}

std::string G::OptionParserImp::qname( char c , const std::string & name )
{
	if( c )
		return {'\"','-',c,'\"'} ;
	else
		return "\"--" + name + "\"" ;
}

void G::OptionParserImp::errorDuplicate( char c , const std::string & name )
{
	error( str( format(txt("duplicate use of %1%")) % qname(c,name) ) ) ;
}

void G::OptionParserImp::errorExtraValue( char c , const std::string & name , const std::string & value )
{
	if( value.empty() )
		error( str( format(txt("cannot give a value with %1%")) % qname(c,name) ) ) ;
	else
		error( str( format(txt("cannot give a value with %1% (%2%)")) % qname(c,name) % value ) ) ;
}

void G::OptionParserImp::errorNoValue( char c , const std::string & name )
{
	error( str( format(txt("no value supplied for %1%")) % qname(c,name) ) ) ;
}

void G::OptionParserImp::errorUnknownOption( char c , const std::string & name )
{
	error( str( format(txt("invalid option: %1%")) % qname(c,name) ) ) ;
}

void G::OptionParserImp::errorConflict( char c , const std::string & name )
{
	error( str( format(txt("conflicting values: %1%")) % qname(c,name) ) ) ;
}

std::string G::OptionParserImp::tooManyArguments()
{
	return txt("too many non-option arguments") ;
}

void G::OptionParserImp::error( const std::string & s )
{
	m_errors.push_back( s ) ;
}

