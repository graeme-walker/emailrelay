//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file submitparser.h
///

#ifndef SUBMIT_PARSER_H
#define SUBMIT_PARSER_H

#include "gdef.h"
#include "gstringarray.h"
#include "gstringview.h"
#include "gexception.h"
#include <vector>
#include <utility>
#include <tuple>
#include <stdexcept>

namespace SubmitParser /// RFC-5322 parser excluding obsolete syntax, with RFC-6532 (UTF-8).
{
	enum class T
	{
		atom ,
		dot_atom ,
		quote ,
		comment ,
		ws ,
		character ,
		error
	} ;
	using Token = std::pair<T,std::string> ;
	using Mailbox = std::tuple<std::string,std::string,std::string> ; // addr-spec or name-addr: local-part, domain, [display-name]
	G_EXCEPTION( Error , tx("rfc-5322/6532 parsing error") )

	std::vector<Token> lex( std::string_view , std::string_view error_more = {} ) ;
		///< Tokenises a header field body according to RFC-5322 with
		///< RFC-6532 (UTF-8). Returns a list of (non-error) tokens.
		///< Throws on error.

	std::string parseMailbox( std::string_view header_field_body , std::string_view error_more = {} ) ;
		///< Parses a mailbox header field body. Throws on error.
		///<
		///< Typically used for RFC-5322 "Sender:" header fields.

	void parseAddress( std::string_view header_field_body , G::StringArray & out , std::string_view error_more = {} ) ;
		///< Parses an address header field body. An 'address' syntax
		///< element can contain multiple addresses by using 'group's.
		///< Throws on error.
		///<
		///< Typically used for RFC-6854 "Sender:" header fields.

	void parseMailboxList( std::string_view header_field_body , G::StringArray & out , std::string_view error_more = {} ) ;
		///< Parses a mailbox-list header field body. Adds mailboxes
		///< to the given array. Throws on error.
		///<
		///< Typically used for RFC-5322 "From:" header fields.

	void parseAddressList( std::string_view header_field_body , G::StringArray & out , bool as_content = false , std::string_view error_more = {} ) ;
		///< Parses an address-list header field body. Adds addresses
		///< to the given array. The emitted addresses can be used
		///< as SMTP envelope addresses. Throws on error.
		///<
		///< Typically used for RFC-5322 "To:,cc:,bcc:" and RFC-6854
		///< "To:,cc:,bcc:,From:" header fields.
}

#endif
