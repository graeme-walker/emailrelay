//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gmsg_win32.cpp
///

#include "gdef.h"
#include "gmsg.h"

ssize_t G::Msg::send( SOCKET fd , const void * buffer , std::size_t size , int flags ) noexcept
{
	return ::send( fd , reinterpret_cast<const char*>(buffer) , static_cast<int>(size) , flags ) ;
}

ssize_t G::Msg::sendto( SOCKET fd , const void * buffer , std::size_t size , int flags ,
	const sockaddr * address_p , socklen_t address_n ) noexcept
{
	return ::sendto( fd , reinterpret_cast<const char*>(buffer) , static_cast<int>(size) ,
		flags , address_p , address_n ) ;
}

ssize_t G::Msg::recv( SOCKET fd , void * buffer , std::size_t size , int flags ) noexcept
{
	return ::recv( fd , reinterpret_cast<char*>(buffer) , static_cast<int>(size) , flags ) ;
}

ssize_t G::Msg::recvfrom( SOCKET fd , void * buffer , std::size_t size , int flags ,
	sockaddr * address_p , socklen_t * address_np ) noexcept
{
	return ::recvfrom( fd , reinterpret_cast<char*>(buffer) , static_cast<int>(size) ,
		flags , address_p , address_np ) ;
}

bool G::Msg::fatal( int error ) noexcept
{
	return !(
		error == 0 ||
		error == WSAEINTR ||
		error == WSAEWOULDBLOCK ||
		error == WSAEINPROGRESS ||
		error == WSAENOBUFS ||
		false ) ;
}

