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
// gpid.h
//

#ifndef G_PID_H
#define G_PID_H

#include "gdef.h"
#include <iostream>

namespace G
{
	class PidImp ;
	class Pid ;
} ;

// Class: G::Pid
// Description: A process-id class. Uses a pimple
// pattern to hide windows/unix type differences.
//
class G::Pid
{
public:
	Pid() ;
		// Default constructor for this
		// process's id.

	~Pid() ;
		// Destructor.

	Pid( const Pid & ) ;
		// Copy constructor.

	Pid & operator=( const Pid & ) ;
		// Assignment operator.

	std::string str() const ;
		// Returns a string representation.

	bool operator==( const Pid & ) const ;
		// Comparison operator.

private:
	PidImp * m_imp ;
} ;

namespace G
{
	inline
	std::ostream & operator<<( std::ostream & stream , const Pid & pid )
	{
		stream << pid.str() ;
		return stream ;
	}
} ;

#endif

