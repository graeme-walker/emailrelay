//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file genvironment_unix.cpp
///

#include "gdef.h"
#include "genvironment.h"
#include <cstdlib> // std::getenv()
#include <cstring>
#include <stdexcept>

namespace G
{
	namespace EnvironmentUnixImp
	{
		char * stringdup( const std::string & ) ;
	}
}

std::string G::Environment::get( const std::string & name , const std::string & default_ )
{
	const char * p = std::getenv( name.c_str() ) ;
	return p ? std::string(p) : default_ ;
}

G::Path G::Environment::getPath( const std::string & name , const G::Path & default_ )
{
	const char * p = std::getenv( name.c_str() ) ;
	return p ? G::Path(p) : default_ ;
}

char * G::EnvironmentUnixImp::stringdup( const std::string & s )
{
	void * p = std::memcpy( new char[s.size()+1U] , s.c_str() , s.size()+1U ) ; // NOLINT
	return static_cast<char*>(p) ;
}

void G::Environment::put( const std::string & name , const std::string & value )
{
	// see man putenv(3) NOTES
	namespace imp = EnvironmentUnixImp ;
	char * deliberately_leaky_copy = imp::stringdup( std::string().append(name).append(1U,'=').append(value) ) ; // NOLINT
	::putenv( deliberately_leaky_copy ) ;
} // NOLINT

G::Environment G::Environment::minimal( bool sbin )
{
	std::string path = sbin ? "/usr/bin:/bin:/usr/sbin:/sbin" : "/usr/bin:/bin" ; // no "."
	return Environment( {{"PATH",path},{"IFS"," \t\n"}} ) ;
}

