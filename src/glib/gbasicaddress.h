//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gbasicaddress.h
///

#ifndef G_BASIC_ADDRESS_H
#define G_BASIC_ADDRESS_H

#include "gdef.h"

namespace G
{
	class BasicAddress ;
}

//| \class G::BasicAddress
/// A structure that holds a network address as a string with no
/// dependency on any low-level network library.
/// \see GNet::Address
///
class G::BasicAddress
{
public:
	explicit BasicAddress( const std::string & s = {} ) ;
		///< Constructor.

	std::string displayString() const ;
		///< Returns a printable string that represents the transport
		///< address.

private:
	std::string m_display_string ;
} ;

inline
G::BasicAddress::BasicAddress( const std::string & s ) :
	m_display_string(s)
{
}

inline
std::string G::BasicAddress::displayString() const
{
	return m_display_string ;
}

#endif
