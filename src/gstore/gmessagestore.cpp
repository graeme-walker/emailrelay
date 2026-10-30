//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gmessagestore.cpp
///

#include "gdef.h"
#include "gmessagestore.h"
#include "gstoredmessage.h"
#include "gstr.h"
#include "gassert.h"
#include "gconvert.h"

std::unique_ptr<GStore::StoredMessage> GStore::operator++( std::shared_ptr<MessageStore::Iterator> & iter )
{
	return iter.get() ? iter->next() : std::unique_ptr<StoredMessage>() ;
}

GStore::MessageStore::AddressStyle GStore::MessageStore::addressStyle( std::string_view address )
{
	if( address.empty() )
		return AddressStyle::Empty ;

	if( address.at(0U) == '@' || address.at(address.size()-1U) == '@' )
		return AddressStyle::Invalid ; // missing stuff

	if( !G::Str::isPrintable(address) )
		return AddressStyle::Invalid ; // control characters (inc. DEL)

	std::size_t at_pos = address.rfind( '@' ) ;
	std::string_view mailbox = G::Str::headView( address , at_pos , address ) ;
	std::string_view domain = G::Str::tailView( address , at_pos ) ;
	G_ASSERT( !mailbox.empty() ) ;

	bool mailbox_ascii = G::Str::isPrintableAscii( mailbox ) ;
	bool mailbox_utf8 = !mailbox_ascii && G::Convert::valid( mailbox ) ;
	bool domain_ascii = domain.empty() || G::Str::isPrintableAscii( domain ) ;
	bool domain_utf8 = !domain_ascii && G::Convert::valid( domain ) ;

	if( ( !mailbox_ascii && !mailbox_utf8 ) || ( !domain_ascii && !domain_utf8 ) )
		return AddressStyle::Invalid ; // invalid encoding
	else if( mailbox_ascii && domain_ascii )
		return AddressStyle::Ascii ;
	else if( mailbox_ascii && !domain_ascii )
		return AddressStyle::Utf8Domain ;
	else if( !mailbox_ascii && domain_ascii )
		return AddressStyle::Utf8Mailbox ;
	else
		return AddressStyle::Utf8Both ;
}

