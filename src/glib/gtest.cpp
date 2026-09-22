//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gtest.cpp
///

#include "gdef.h"
#include "gtest.h"
#include "glog.h"
#include "genvironment.h"
#include <string>
#include <set>

#if defined(_DEBUG) || defined(G_TEST_ENABLED)
namespace G
{
	namespace TestImp
	{
		bool spec_set = false ;
		std::string spec( bool set = false , const std::string & s = std::string() ) ;
	}
}
std::string G::TestImp::spec( bool set , const std::string & s_in )
{
	static std::string s ;
	if( set )
	{
		if( !s_in.empty() )
			s = "," + s_in + "," ;
		spec_set = true ;
	}
	return s ;
}
void G::Test::set( const std::string & s )
{
	TestImp::spec( true , s ) ;
}
bool G::Test::enabled( const char * name )
{
	if( !TestImp::spec_set )
	{
		TestImp::spec( true , Environment::get("G_TEST",std::string()) ) ;
	}

	bool result = TestImp::spec().empty() ? false : ( TestImp::spec().find(","+std::string(name)+",") != std::string::npos ) ;
	if( result )
	{
		static std::set<std::string> warned ;
		if( warned.find(name) == warned.end() )
		{
			warned.insert( name ) ;
			G_WARNING( "G::Test::enabled: test case enabled: [" << name << "]" ) ;
		}
	}
	return result ;
}
bool G::Test::enabled() noexcept
{
	return true ;
}
#else
void G::Test::set( const std::string & )
{
}
bool G::Test::enabled() noexcept
{
	return false ;
}
#endif
