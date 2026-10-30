//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gmsg_mac.cpp
///

#include "gdef.h"
#include "gmsg.h"
#include <cerrno> // EINTR etc
#include <sys/types.h>
#include <sys/socket.h>

ssize_t G::Msg::send( int fd , const void * buffer , std::size_t size , int flags ) noexcept
{
	return sendto( fd , buffer , size , flags , nullptr , 0U ) ;
}

ssize_t G::Msg::sendto( int fd , const void * buffer , std::size_t size , int flags ,
	const sockaddr * address_p , socklen_t address_n ) noexcept
{
	return ::sendto( fd , buffer , size , flags|MSG_NOSIGNAL , // NOLINT
		const_cast<sockaddr*>(address_p) , address_n ) ;
}

ssize_t G::Msg::recv( int fd , void * buffer , std::size_t size , int flags ) noexcept
{
	return ::recv( fd , buffer , size , flags ) ;
}

ssize_t G::Msg::recvfrom( int fd , void * buffer , std::size_t size , int flags ,
	sockaddr * address_p , socklen_t * address_np ) noexcept
{
	return ::recvfrom( fd , buffer , size , flags , address_p , address_np ) ;
}

bool G::Msg::fatal( int error ) noexcept
{
	return !(
		error == 0 ||
		error == EAGAIN ||
		error == EINTR ||
		error == EMSGSIZE || // moot
		error == ENOBUFS ||
		error == ENOMEM ) ;
}

