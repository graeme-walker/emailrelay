//
// commandline_unix.cpp
//

#include "gdef.h"
#include "gsmtp.h"
#include "commandline.h"
#include "gstr.h"
#include <iostream>

Main::CommandLine::Show * Main::CommandLine::Show::m_this = NULL ;

Main::CommandLine::Show::Show( bool e ) :
	m_e(e)
{
}

std::ostream & Main::CommandLine::Show::s()
{
	return m_e ? std::cerr : std::cout ;
}

Main::CommandLine::Show::~Show()
{
}

unsigned int Main::CommandLine::ttyColumns() const
{
	const unsigned int default_ = 79U ;
	try
	{
		const char * p = std::getenv( "COLUMNS" ) ;
		if( p == NULL )
			return default_ ;
		else
			return G::Str::toUInt(p) ;
	}
	catch( std::exception & )
	{
		return default_ ;
	}
}

