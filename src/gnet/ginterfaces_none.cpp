//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file ginterfaces_none.cpp
///

#include "gdef.h"
#include "ginterfaces.h"
#include "gstr.h"
#include "gassert.h"

GNet::Interfaces::Interfaces( EventState es ) :
	m_es(es)
{
}

GNet::Interfaces::Interfaces( EventState es , InterfacesHandler & ) :
	m_es(es)
{
}

GNet::Interfaces::~Interfaces()
= default;

bool GNet::Interfaces::supported()
{
	return false ;
}

bool GNet::Interfaces::active()
{
	return false ;
}

void GNet::Interfaces::load()
{
}

bool GNet::Interfaces::loaded() const
{
	return true ;
}

G::StringArray GNet::Interfaces::names( bool ) const
{
	return {} ;
}

GNet::Interfaces::const_iterator GNet::Interfaces::begin() const
{
	return m_list.begin() ;
}

GNet::Interfaces::const_iterator GNet::Interfaces::end() const
{
	return m_list.end() ;
}

std::size_t GNet::Interfaces::addresses( std::vector<GNet::Address> & , const std::string & , unsigned int , int ) const
{
	return 0U ;
}

std::vector<GNet::Address> GNet::Interfaces::addresses( const std::string & , unsigned int , int ) const
{
	return {} ;
}

std::size_t GNet::Interfaces::addresses( std::vector<Address> & , const std::string & , const std::vector<unsigned> & , int ) const
{
	return 0U ;
}

void GNet::Interfaces::readEvent()
{
}

void GNet::Interfaces::onFutureEvent()
{
}

// ==

GNet::Interfaces::Item::Item() :
	address(Address::defaultAddress())
{
}

