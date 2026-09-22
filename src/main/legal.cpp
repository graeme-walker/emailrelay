//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file legal.cpp
///

#include "gdef.h"
#include "legal.h"
#include <string>
#include <sstream>

std::string Main::Legal::copyright()
{
	return "Copyright (C) 2026 Graeme Walker" ;
}

std::string Main::Legal::warranty( const std::string & prefix , const std::string & eol )
{
	std::ostringstream ss ;
	ss
		<< prefix << "This program comes with ABSOLUTELY NO WARRANTY." << eol
		<< prefix << "This is free software, and you are welcome to " << eol
		<< prefix << "redistribute it under certain conditions. For " << eol
		<< prefix << "more information refer to the file named COPYING." << eol ;
	return ss.str() ;
}

