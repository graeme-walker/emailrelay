//
// Copyright (C) 2001-2002 Graeme Walker <graeme_walker@users.sourceforge.net>
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
// passwd.cpp
//
// A utility which encrypts a password so that
// it can be pasted into the emailrelay secrets
// file(s).
//

#include "gdef.h"
#include "gstr.h"
#include "garg.h"
#include "gmd5.h"
#include <iostream>

int main( int argc , char * argv [] )
{
	G::Arg arg( argc , argv ) ;
	try
	{
		if( argc != 1 )
		{
			std::cerr
				<< "usage: " << arg.prefix() << std::endl
				<< "  This software is provided without warranty of any kind." << std::endl
				<< "  You may redistribure copies of this program under " << std::endl
				<< "  the terms of the GNU General Public License." << std::endl
				<< "  For more information refer to the file named COPYING." << std::endl
        			<< "    Copyright (C) 2002 Graeme Walker" << std::endl ;
			return EXIT_FAILURE ;
		}

		std::string key = G::Str::readLineFrom( std::cin ) ;
		G::Str::trim( key , " \t\n\r" ) ;
		if( key.length() == 0U )
		{
			std::cerr << arg.prefix() << ": invalid password" << std::endl ;
			return EXIT_FAILURE ;
		}

		std::cout << G::Md5::mask(key) << std::endl ;
		return EXIT_SUCCESS ;
	}
	catch( std::exception & e )
	{
		std::cerr << arg.prefix() << ": exception: " << e.what() << std::endl ;
	}
	catch( ... )
	{
		std::cerr << arg.prefix() << ": unknown exception" << std::endl ;
	}
	return EXIT_FAILURE ;
}

