//
// Copyright (C) 2001 Graeme Walker <graeme_walker@users.sourceforge.net>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
// ===
//
// gpid_unix.cpp
//

#include "gdef.h"
#include "gpid.h"
#include <unistd.h>
#include <sys/types.h>
#include <sstream>

namespace G
{
	class PidImp ;
} ;

// Class: G::PidImp
// Description: A pimple implementation class for GPid.
//
class G::PidImp
{
public:
	pid_t m_pid ;
} ;

// ===

G::Pid::Pid() : m_imp(NULL)
{
	m_imp = new PidImp ;
	m_imp->m_pid = ::getpid() ;
}

G::Pid::~Pid()
{
	delete m_imp ;
}

G::Pid::Pid( const Pid & other ) :
	m_imp(NULL)
{
	m_imp = new PidImp ;
	m_imp->m_pid = other.m_imp->m_pid ;
}

G::Pid & G::Pid::operator=( const Pid & rhs )
{
	m_imp->m_pid = rhs.m_imp->m_pid ;
	return *this ;
}

std::string G::Pid::str() const
{
	std::stringstream ss ;
	ss << m_imp->m_pid ;
	return ss.str() ;
}

bool G::Pid::operator==( const Pid & rhs ) const
{
	return m_imp->m_pid == rhs.m_imp->m_pid ;
}

