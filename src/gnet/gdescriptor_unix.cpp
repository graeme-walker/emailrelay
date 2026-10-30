//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gdescriptor_unix.cpp
///

#include "gdef.h"
#include "gdescriptor.h"

GNet::Descriptor::Descriptor() noexcept :
	m_fd(-1)
{
}

bool GNet::Descriptor::validfd() const noexcept
{
	return m_fd >= 0 ;
}

HANDLE GNet::Descriptor::h() const noexcept
{
	return HNULL ;
}

void GNet::Descriptor::streamOut( std::ostream & stream ) const
{
	stream << m_fd ;
}

