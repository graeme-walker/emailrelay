//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gthread.cpp
///

#include "gdef.h"

namespace G
{
	namespace ThreadImp
	{
		void test_fn() {}
	}
}

bool G::threading::works()
{
	if( using_std_thread )
	{
		static bool first = true ;
		static bool result = false ;
		if( first )
		{
			first = false ;
			try
			{
				threading::thread_type t( ThreadImp::test_fn ) ;
				t.join() ;
				threading::mutex_type mutex ;
				threading::lock_type lock( mutex ) ;
				result = true ;
			}
			catch(...)
			{
				// eg. gcc std::thread builds okay with -std=c++11 but throws
				// at run-time if not also built with "-pthread" -- also, linking
				// with -lGL suppresses linking with libpthread.so and breaks
				// threading at run-time -- also, gcc 4.8 bugs
				result = false ;
			}
		}
		return result ;
	}
	else
	{
		return false ;
	}
}

