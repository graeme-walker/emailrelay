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
// ggetopt.h
//	

#ifndef G_GETOPT_H
#define G_GETOPT_H

#include "gdef.h"
#include "garg.h"
#include "gstrings.h"
#include "gexception.h"
#include <string>
#include <list>
#include <map>

namespace G
{
	class GetOpt ;
} ;

// Class: G::GetOpt
// Description: A command line switch parser.
// See also: G::Arg
//
class G::GetOpt
{
public:
	typedef std::vector<std::string GAllocator(std::string) > StringArray ;
	G_EXCEPTION( InvalidSpecification , "invalid options specification string" ) ;

	GetOpt( const Arg & arg , const std::string & spec ,
		char sep_major = '|' , char sep_minor = '/' , char escape = '\\' ) ;
			// Constructor taking a Arg reference and a
			// specification string. Supports old-fashioned
			// getopt specification strings such as "p:dv", and
			// also new-stye specifications like
			// "p/port/port number/1|d/debug/show debug/0|v/verbose/show more/0".
			// In the new-style specification each switch definition
			// is made up of the following...
			//    <single-character-switch-letter>
			//    <multi-character-switch-name>
			//    <switch-description>
			//    <value-type> -- 0 is none, and 1 is a string
			//    <value-description>
			//
			// If the switch-description field is empty
			// then the switch is hidden.

	Arg args() const ;
		// Returns all the non-switch command-line arguments.

	Strings errorList() const ;
		// Returns the list of errors.

	static size_t wrapDefault() ;
		// Returns a default word-wrapping width.

	std::string usageSummary( const std::string & exe , const std::string & args ,
		size_t wrap_width = wrapDefault() ) const ;
			// Returns a one-line usage summary, as
			// "usage: <exe> <usageSummarySwitches()> <args>"

	std::string usageSummarySwitches() const ;
		// Returns the one-line summary of switches. Does _not_
		// include the usual "usage: <exe>" prefix
		// or non-switch arguments.

	std::string usageHelp( size_t tab_stop = 30U , size_t wrap_width = wrapDefault() ) const ;
		// Returns a multi-line string giving help on each switch.

	void showUsage( std::ostream & stream , const std::string & exe ,
		const std::string & args , size_t tab_stop = 30U ,
		size_t wrap_width = wrapDefault() ) const ;
			// Streams out multi-line usage text using
			// usageSummary() and usageHelp(). Does nothing
			// about non-switch arguments.

	bool hasErrors() const ;
		// Returns true if there are errors.

	void showErrors( std::ostream & stream , std::string prefix_1 ,
		std::string prefix_2 = std::string(": ") ) const ;
			// A convenience function which streams out each errorList()
			// item to the given stream, prefixed with the given
			// prefix(es). The two prefixes are simply concatenated.

	void show( std::ostream & stream , std::string prefix ) const ;
		// For debugging.

	bool contains( char switch_letter ) const ;
		// Returns true if the command line contains
		// the given switch.

	bool contains( const std::string & switch_name ) const ;
		// Returns true if the command line contains
		// the given switch.

	std::string value( const std::string & switch_name ) const ;
		// Returns the value related to the given
		// value-based switch.

	std::string value( char switch_letter ) const ;
		// Returns the value related to the given
		// value-based switch.

private:
	struct SwitchSpec
	{
		char c ;
		std::string name ;
		std::string description ;
		bool valued ;
		bool hidden ;
		std::string value_description ;
		SwitchSpec(char c_,const std::string &name_,const std::string &description_,
			bool v_,const std::string &vd_) :
				c(c_) , name(name_) , description(description_) ,
				hidden(description_.empty()) ,
				valued(v_) , value_description(vd_) {}
	} ;
	typedef std::map<unsigned int,SwitchSpec GLessAllocator(char,SwitchSpec) > SwitchSpecMap ;
	typedef std::pair<bool,std::string> Value ;
	typedef std::map<char,Value GLessAllocator(char,Value) > SwitchMap ;

	void operator=( const GetOpt & ) ;
	GetOpt( const GetOpt & ) ;
	void parseSpec( const std::string & spec , char , char , char ) ;
	void parseOldSpec( const std::string & spec ) ;
	void parseNewSpec( const std::string & spec , char , char , char ) ;
	void addSpec( unsigned int ordinal , char c , bool valued ) ;
	void addSpec( unsigned int ordinal , char c , const std::string & name , const std::string & , bool valued , const std::string & ) ;
	size_t parseArgs( const Arg & args_in ) ;
	bool isOldSwitch( const std::string & arg ) const ;
	bool isNewSwitch( const std::string & arg ) const ;
	bool isSwitchSet( const std::string & arg ) const ;
	void processSwitch( char c ) ;
	void processSwitch( char c , const std::string & value ) ;
	void processSwitch( const std::string & s ) ;
	void processSwitch( const std::string & s , const std::string & value ) ;
	bool valued( char c ) const ;
	bool valued( const std::string & ) const ;
	void errorNoValue( char c ) ;
	void errorNoValue( const std::string & ) ;
	void errorUnknownSwitch( char c ) ;
	void errorUnknownSwitch( const std::string & ) ;
	char key( const std::string & s ) const ;
	void remove( size_t n ) ;
	bool valid( const std::string & ) const ;
	bool valid( char c ) const ;
	std::string usageSummaryPartOne() const ;
	std::string usageSummaryPartTwo() const ;
	std::string usageHelpCore( const std::string & , size_t , size_t ) const ;
	static size_t widthLimit( size_t ) ;

private:
	SwitchSpecMap m_spec_map ;
	SwitchMap m_map ;
	Strings m_errors ;
	Arg m_args ;
} ;

#endif
