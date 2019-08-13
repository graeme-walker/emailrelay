//
// Copyright (C) 2001-2018 Graeme Walker <graeme_walker@users.sourceforge.net>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// ===
//
// gexecutablecommand_win32.cpp
//

#include "gdef.h"
#include "gexecutablecommand.h"
#include "glimits.h"
#include "gstr.h"
#include <stdexcept>
#include <algorithm>

G::StringArray G::ExecutableCommand::osSplit( const std::string & s_in )
{
	std::string s( s_in ) ;

	// mark escaped spaces using nul -- assume no directory name starts with a space
	const std::string null( 1U , '\0' ) ;
	G::Str::replaceAll( s , "\\ " , null ) ;

	// split up on (unescaped) spaces
	G::StringArray parts ;
	G::Str::splitIntoTokens( s , parts , " " ) ;

	// replace the escaped spaces
	for( G::StringArray::iterator p = parts.begin() ; p != parts.end() ; ++p )
	{
		G::Str::replaceAll( *p , null , " " ) ;
	}

	return parts ;
}

bool G::ExecutableCommand::osNativelyRunnable() const
{
	std::string type = G::Str::lower(m_exe.extension()) ;
	return type == "exe" || type == "bat" ;
}

void G::ExecutableCommand::osAddWrapper()
{
	// use "<windows>/system32/cscript.exe" -- perhaps it would be
	// better to do assoc/ftype and add "/H:CScript" if ends up as
	// "wscript.exe" -- but in any case this is only a convenience
	// that the user is free to specify explicitly

	std::string windows ;
	{
		char buffer[MAX_PATH] = { 0 } ;
		unsigned int n = ::GetWindowsDirectoryA( buffer , MAX_PATH ) ;
		if( n == 0 || n > MAX_PATH )
			throw std::runtime_error( "cannot determine the windows directory" ) ;
		windows = std::string( buffer , n ) ;
	}

	// m_exe=<exe>, m_args=[<arg> ...]
	// m_exe="cscript.exe" m_args=["//nologo" "//B" <exe> <arg> ...]

	std::reverse( m_args.begin() , m_args.end() ) ;
	{
		m_args.push_back( m_exe.str() ) ;
		m_args.push_back( "//B" ) ;
		m_args.push_back( "//nologo" ) ;
	}
	std::reverse( m_args.begin() , m_args.end() ) ;

	m_exe = G::Path( windows , "system32" , "cscript.exe" ) ;
}

/// \file gexecutablecommand_win32.cpp