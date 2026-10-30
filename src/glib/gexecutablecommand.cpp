//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gexecutablecommand.cpp
///

#include "gdef.h"
#include "gexecutablecommand.h"
#include "garg.h"
#include "gstr.h"
#include <algorithm>
#include <iterator>

G::ExecutableCommand::ExecutableCommand( const std::string & s )
{
	if( !s.empty() )
	{
		m_args = Arg(s).array() ;
		m_exe = m_args.at(0U) ;
		std::rotate( m_args.begin() , m_args.begin()+1U , m_args.end() ) ;
		m_args.pop_back() ; // remove exe
	}
}

G::ExecutableCommand::ExecutableCommand( const G::Path & exe_ , const G::StringArray & args_ ) :
	m_exe(exe_) ,
	m_args(args_)
{
}

G::Path G::ExecutableCommand::exe() const
{
	return m_exe ;
}

G::StringArray G::ExecutableCommand::args() const
{
	return m_args ;
}

std::string G::ExecutableCommand::displayString() const
{
	return
		m_args.empty() ?
			std::string("[") + m_exe.str() + "]" :
			std::string("[") + m_exe.str() + "] [" + Str::join("] [",m_args) + "]" ;
}

void G::ExecutableCommand::add( const std::string & arg )
{
	m_args.push_back( arg ) ;
}

void G::ExecutableCommand::insert( const G::StringArray & array )
{
	if( !array.empty() )
	{
		m_args.insert( m_args.begin() , m_exe.str() ) ;
		m_args.insert( m_args.begin() , std::next(array.begin()) , array.end() ) ;
		m_exe = m_args.at( 0U ) ;
	}
}

