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
// main.cpp
//

#include "gdef.h"
#include "gsmtp.h"
#include "garg.h"
#include "run.h"
#include <exception>

int main( int argc , char * argv [] )
{
	G::Arg arg( argc , argv ) ;
	try
	{
		Main::Run main( arg ) ;
		if( main.prepare() )
			main.run() ;
		return EXIT_SUCCESS ;
	}
	catch( std::exception & e )
	{
		std::cerr << arg.prefix() << ": exception: " << e.what() << std::endl ;
	}
	catch( ... )
	{
		std::cerr << arg.prefix() << ": unrecognised exception" << std::endl ;
	}
	return EXIT_FAILURE ;
}
