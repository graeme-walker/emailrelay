//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file glogstream.cpp
///

#include "gdef.h"
#include "glogstream.h"
#include <ostream>

G::LogStream & G::operator<<( LogStream & s , const std::string & value ) noexcept
{
	try
	{
		if( s.m_ostream ) *(s.m_ostream) << value ;
	}
	catch(...)
	{
	}
	return s ;
}

G::LogStream & G::operator<<( LogStream & s , const char * value ) noexcept
{
	try
	{
		if( s.m_ostream ) *(s.m_ostream) << value ;
	}
	catch(...)
	{
	}
	return s ;
}

G::LogStream & G::operator<<( LogStream & s , char value ) noexcept
{
	try
	{
		if( s.m_ostream ) *(s.m_ostream) << value ;
	}
	catch(...)
	{
	}
	return s ;
}

G::LogStream & G::operator<<( LogStream & s , unsigned char value ) noexcept
{
	try
	{
		if( s.m_ostream ) *(s.m_ostream) << value ;
	}
	catch(...)
	{
	}
	return s ;
}

G::LogStream & G::operator<<( LogStream & s , int value ) noexcept
{
	try
	{
		if( s.m_ostream ) *(s.m_ostream) << value ;
	}
	catch(...)
	{
	}
	return s ;
}

G::LogStream & G::operator<<( LogStream & s , unsigned int value ) noexcept
{
	try
	{
		if( s.m_ostream ) *(s.m_ostream) << value ;
	}
	catch(...)
	{
	}
	return s ;
}

G::LogStream & G::operator<<( LogStream & s , long value ) noexcept
{
	try
	{
		if( s.m_ostream ) *(s.m_ostream) << value ;
	}
	catch(...)
	{
	}
	return s ;
}

G::LogStream & G::operator<<( LogStream & s , unsigned long value ) noexcept
{
	try
	{
		if( s.m_ostream ) *(s.m_ostream) << value ;
	}
	catch(...)
	{
	}
	return s ;
}

G::LogStream & G::operator<<( LogStream & s , void * p ) noexcept // inc. HANDLE
{
	try
	{
		if( s.m_ostream ) *(s.m_ostream) << reinterpret_cast<g_uintptr_t>(p) ;
	}
	catch(...)
	{
	}
	return s ;
}

