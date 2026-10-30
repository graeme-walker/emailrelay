//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gpam_none.cpp
///

#include "gdef.h"
#include "gpam.h"
#include <string>

class G::PamImp
{
} ;

G::Pam::Pam( const std::string & , const std::string & , bool )
{
}

G::Pam::~Pam()
= default;

bool G::Pam::authenticate( bool )
{
	throw Error( "authenticate" , 0 ) ;
}

void G::Pam::checkAccount( bool )
{
}

void G::Pam::establishCredentials()
{
}

void G::Pam::openSession()
{
}

void G::Pam::closeSession()
{
}

void G::Pam::deleteCredentials()
{
}

void G::Pam::reinitialiseCredentials()
{
}

void G::Pam::refreshCredentials()
{
}

std::string G::Pam::name() const
{
	return {} ;
}

