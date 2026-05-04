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
/// \file goptionparser.h
///

#ifndef G_OPTION_PARSER_H
#define G_OPTION_PARSER_H

#include "gdef.h"
#include "gstringarray.h"
#include "goptions.h"
#include "goptionvalue.h"
#include "goptionmap.h"
#include <string>
#include <vector>
#include <utility>
#include <functional>

namespace G
{
	class OptionParser ;
}

//| \class G::OptionParser
/// A parser for command-line arguments that operates according to an Options
/// specification and returns an OptionValue multimap.
/// \see G::Options, G::OptionValue
///
class G::OptionParser
{
public:
	using CallbackFn = std::function<std::string(const std::string&,const std::string&)> ;
	using Maps = std::vector<std::pair<std::string,OptionMap>> ;
	using StringPair = std::pair<std::string,std::string> ;
	struct Config
	{
		std::size_t start_position {0U} ;
		std::size_t ignore_non_options {0U} ;
		bool keep_negatives {false} ;
		Config & set_start_position( std::size_t n ) noexcept ;
		Config & set_ignore_non_options( std::size_t n ) noexcept ;
		Config & set_keep_negatives( bool b = true ) noexcept ;
	} ;

	static StringArray parse( const StringArray & args_in , const Options & spec ,
		OptionMap & value_map_out , StringArray & errors_out , Config ,
		CallbackFn callback_fn = {} ) ;
			///< Parses the given command-line arguments into the value map
			///< and/or error list and returns the non-option arguments. The
			///< outputs are not cleared so this function can be called
			///< more than once, typically using different callback functions.
			///<
			///< Parsing stops at the first non-option or at "--" and the
			///< remaining non-options are returned. Optionally some number of
			///< non-options are tolerated without stopping the parsing if
			///< 'ignore_non_options' is non-zero. This is typically to allow
			///< for sub-commands like "exe subcmd --opt arg" or possibly
			///< "exe --opt1 subcmd --opt2 arg".
			///<
			///< Individual arguments can be in short-form like "-c", or long-form
			///< like "--foo" or "--foo=bar". Long-form arguments can be passed in
			///< two separate arguments, eg. "--foo" followed by "bar". Short-form
			///< options can be grouped (eg. "-abc"). Boolean options can be enabled
			///< by (eg.) "--verbose" or "--verbose=yes", and disabled by "--verbose=no".
			///< Boolean options cannot use two separate arguments (eg. "--verbose"
			///< followed by "yes").
			///<
			///< Defaulting options can have have a value or not, but when there
			///< is a value it must use the 'equals' form rather than a separate
			///< argument. So "--foo=" and "--foo" are equivalent, having an empty
			///< value, and "--foo=bar" has a value of 'bar', but "--foo bar" is
			///< interpreted as 'foo' taking its default (empty) value followed
			///< by a separate argument 'bar'.
			///<
			///< Parsing errors are appended to the given error list, but without
			///< de-duplication. It is normally sufficient to report the first
			///< error.
			///<
			///< Entries in the output map are keyed by the option's long name,
			///< even if supplied in short-form.
			///<
			///< The optional callback function can be used to modify the parsing
			///< of each option. The callback is passed the string after the
			///< double-dash (or the option's long name if originally short form)
			///< and it must return the option name to be used for parsing. The
			///< callback is also passed a one-character string that can be
			///< added to the front of the returned string to indicate that the
			///< results should be discarded rather than being added to the
			///< OptionMap.

	static StringArray parse( const StringArray & args_in , const Options & spec ,
		const StringArray & names , Maps & value_maps_out ,
		StringArray & errors_out , Config ) ;
			///< An overload that parses command-line options that can have an
			///< optional section prefix, like "--<section>-<option>=<value>".
			///< The section names should normally come from names(). Options
			///< without a section name are put into the first map.

	static StringArray names( const StringArray & args_in , const Options & spec ,
		StringArray & errors_out , Config , std::string_view introducer ) ;
			///< Returns the list of section names. A new section name is
			///< recognised via a command-line option like
			///< "--<new-section>-<introducer>[=<value>]".
			///<
			///< Returns an empty list if there are parsing errors. Returns a
			///< single-element list containing the empty string if there are
			///< no names.

	static StringPair file( const StringArray & args_in , const Options & spec ,
		StringArray & errors_out , Config config , std::string_view modifier = {} ) ;
			///< Parses the command-line options to identity non-option
			///< command-line arguments and then returns (first) the first
			///< command-line argument as a config file. Fails if there is
			///< more that one non-option argument. Also optionally returns
			///< (second) the value of a special command-line option that
			///< modifies how the config file is to be processed.

public:
	OptionParser() = delete ;
} ;

// clang-format off
inline G::OptionParser::Config & G::OptionParser::Config::set_start_position( std::size_t n ) noexcept { start_position = n ; return *this ; }
inline G::OptionParser::Config & G::OptionParser::Config::set_ignore_non_options( std::size_t n ) noexcept { ignore_non_options = n ; return *this ; }
inline G::OptionParser::Config & G::OptionParser::Config::set_keep_negatives( bool b ) noexcept { keep_negatives = b ; return *this ; }
// clang-format on

#endif
