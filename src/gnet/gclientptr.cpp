//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gclientptr.cpp
///

#include "gdef.h"
#include "gclientptr.h"

GNet::ClientPtrBase::ClientPtrBase()
= default;

G::Slot::Signal<const std::string&> & GNet::ClientPtrBase::deletedSignal() noexcept
{
	return m_deleted_signal ;
}

G::Slot::Signal<const std::string&,const std::string&,const std::string&> & GNet::ClientPtrBase::eventSignal() noexcept
{
	return m_event_signal ;
}

G::Slot::Signal<const std::string&> & GNet::ClientPtrBase::deleteSignal() noexcept
{
	return m_delete_signal ;
}

void GNet::ClientPtrBase::eventSlot( const std::string & s1 , const std::string & s2 , const std::string & s3 )
{
	m_event_signal.emit( std::string(s1) , std::string(s2) , std::string(s3) ) ;
}

