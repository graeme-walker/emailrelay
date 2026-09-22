//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file goptionreader.h
///

#ifndef G_OPTION_READER_H
#define G_OPTION_READER_H

#include "gdef.h"
#include "gstringarray.h"
#include "gexception.h"
#include "ggettext.h"
#include "gpath.h"

namespace G
{
	class OptionReader ;
}

//| \class G::OptionReader
/// Provides a static function to read options from a config file.
///
class G::OptionReader
{
public:
	G_EXCEPTION( FileError , tx("error reading configuration file") )

	static StringArray read( const G::Path & , std::size_t limit = 1000U ) ;
		///< Reads options from file as a list of strings like "--foo=bar".
		///< Throws on error.

	static std::size_t add( StringArray & out , const G::Path & , std::size_t limit = 1000U ) ;
		///< Adds options read from file to an existing list.
		///< Returns the number of options added.

public:
	OptionReader() = delete ;
} ;

#endif
