//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gcleanup_win32.cpp
///

#include "gdef.h"
#include "gcleanup.h"
#include <cstring> // _strdup()

void G::Cleanup::init()
{
	// no-op
}

void G::Cleanup::add( bool (*)(const Arg &) noexcept , Arg )
{
	// not implemented
}

void G::Cleanup::atexit( bool )
{
	// not implemented
}

void G::Cleanup::block() noexcept
{
	// not implemented
}

void G::Cleanup::release() noexcept
{
	// not implemented
}

G::Cleanup::Arg G::Cleanup::arg( const char * )
{
	return {} ;
}

G::Cleanup::Arg G::Cleanup::arg( const std::string & )
{
	return {} ;
}

G::Cleanup::Arg G::Cleanup::arg( const Path & )
{
	Arg arg ;
	//arg.m_is_path = true ; // fwiw
	return arg ;
}

G::Cleanup::Arg G::Cleanup::arg( std::nullptr_t )
{
	return {} ;
}

const char * G::Cleanup::Arg::str() const noexcept
{
	return m_ptr ;
}

bool G::Cleanup::Arg::isPath() const noexcept
{
	return m_is_path ;
}

