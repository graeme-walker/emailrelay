//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gaddress6.h
///

#ifndef G_NET_ADDRESS6_H
#define G_NET_ADDRESS6_H

#include "gdef.h"
#include "gaddress.h"
#include "gstringview.h"
#include <string>

namespace GNet
{
	class Address6 ;
}

//| \class GNet::Address6
/// A 'sockaddr' wrapper class for IPv6 addresses.
///
class GNet::Address6
{
public:
	using sockaddr_type = sockaddr_in6 ;

	explicit Address6( unsigned int ) ;
	explicit Address6( std::string_view ) ;
	Address6( std::string_view , std::string_view ) ;
	Address6( unsigned int port , int /*for overload resolution*/ ) ; // canonical loopback address
	Address6( const sockaddr * addr , socklen_t len , bool ipv6_scope_id_fixup = false ) ;

	static int domain() noexcept ;
	static unsigned short af() noexcept ;
	const sockaddr * address() const ;
	sockaddr * address() ;
	static socklen_t length() noexcept ;
	unsigned long scopeId( unsigned long default_ = 0UL ) const ;
	unsigned int port() const ;
	void setPort( unsigned int port ) ;
	bool setZone( std::string_view ipv6_zone_name_or_scope_id ) ;
	void setScopeId( unsigned long ipv6_scope_id ) ;
	static bool validString( std::string_view , std::string * = nullptr ) ;
	static bool validStrings( std::string_view , std::string_view , std::string * = nullptr ) ;
	static bool validPort( unsigned int port ) ;
	static bool validData( const sockaddr * addr , socklen_t len ) ;

	bool same( const Address6 & other , bool ipv6_compare_with_scope = false ) const ;
	bool sameHostPart( const Address6 & other , bool ipv6_compare_with_scope = false ) const ;
	bool isLoopback() const ;
	bool isLocal( std::string & ) const ;
	bool isLinkLocal() const ;
	bool isUniqueLocal() const ;
	bool isMulticast() const ;
	bool isAny() const ;
	unsigned int bits() const ;
	std::string displayString( bool ipv6_with_scope = false ) const ;
	std::string hostPartString() const ;
	std::string queryString() const ;
	G::StringArray wildcards() const ;

private:
	explicit Address6( std::nullptr_t ) ;
	static const char * setAddress( sockaddr_type & , std::string_view ) ;
	static const char * setHostAddress( sockaddr_type & , std::string_view ) ;
	static const char * setPort( sockaddr_type & , unsigned int ) ;
	static const char * setPort( sockaddr_type & , std::string_view ) ;
	static bool sameAddr( const ::in6_addr & a , const ::in6_addr & b ) ;
	static bool setZone( sockaddr_type & , std::string_view ) ;

private:
	sockaddr_type m_inet ;
} ;

#endif
