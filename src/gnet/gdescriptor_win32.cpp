//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gdescriptor_win32.cpp
///

#include "gdef.h"
#include "gdescriptor.h"

GNet::Descriptor::Descriptor() noexcept :
	m_fd(INVALID_SOCKET)
{
}

bool GNet::Descriptor::validfd() const noexcept
{
	return m_fd != INVALID_SOCKET ;
}

HANDLE GNet::Descriptor::h() const noexcept
{
	return m_handle ;
}

void GNet::Descriptor::streamOut( std::ostream & stream ) const
{
	if( m_fd == INVALID_SOCKET )
		stream << "-1" ;
	else
		stream << m_fd ;

	stream << "," << m_handle ;
}

