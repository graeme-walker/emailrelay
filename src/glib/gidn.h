//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gidn.h
///

#ifndef G_IDN_H
#define G_IDN_H

#include "gdef.h"
#include "gstringview.h"
#include "gexception.h"
#include <string>

namespace G
{
	namespace Idn /// Internationalised Domain Name encoding.
	{
		G_EXCEPTION_CLASS( Error , tx("error encoding idn domain name") )

		bool valid( std::string_view domain ) ;
			///< Returns true if the given domain is valid with
			///< U-labels and/or A-labels.

		std::string encode( std::string_view domain ) ;
			///< Returns the given domain with A-lables.
			///< Precondition: valid(domain)
	}
}

#endif
