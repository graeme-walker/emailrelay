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
// gdef.h
//

// This header is always the first header included in source
// files. It takes care of some portability issues, and
// is a good candidate for precompilation. It requires
// either G_UNIX or G_WIN32 to be defined on the compiler
// command line, although G_UNIX may also be inferred from
// autoconf's HAVE_CONFIG_H.
//

#ifndef G_DEF_H
#define G_DEF_H

	// Autoconf stuff
	//
	#if defined(HAVE_CONFIG_H)

		#include <config.h>

		#if ! HAVE_GMTIME_R
			#include <ctime>
			namespace std
			{
				inline struct tm * gmtime_r( const time_t * tp , struct tm * tm_p )
				{
					* tm_p = * gmtime( tp ) ;
					return tm_p ;
				}
			} ;
		#endif

		#if ! HAVE_LOCALTIME_R
			#include <ctime>
			namespace std
			{
				inline struct tm * localtime_r( const time_t * tp , struct tm * tm_p )
				{
					* tm_p = * localtime( tp ) ;
					return tm_p ;
				}
			} ;
		#endif

		#if ! defined( G_UNIX )
			#define G_UNIX
		#endif
	#endif

	// Check operating-system switches
	//
	#if !defined( G_WIN32 ) && !defined( G_UNIX )
		#error invalid compilation switches
	#endif
	#if defined( G_WIN32 ) && defined( G_UNIX )
		#error invalid compilation switches
	#endif

	// Define supplementary o/s compilation switches
	//
	#if defined( G_WIN32 ) && ! defined( G_WINDOWS )
		#define G_WINDOWS
	#endif

	// Define the compiler
	//
	#if defined( _MSC_VER )
		#define G_COMPILER_IS_MICROSOFT 1
	#endif
	#if defined( __GNUC__ )
		#define G_COMPILER_IS_GNU 1
	#endif

	// Include main o/s headers
	//
	#if defined( G_WINDOWS )
		#include <windows.h>
		#include <shellapi.h>
	#else
		#include <unistd.h>
		#include <sys/stat.h>
	#endif

	// Define Windows-style types (only used for
	// unimplemented declarations under unix)
	//
	#if ! defined( G_WINDOWS )
		typedef unsigned char BOOL ;
		typedef unsigned int HWND ;
		typedef unsigned int HINSTANCE ;
		typedef unsigned int HANDLE ;
	#endif

	// Include commonly-used system headers (good for
	// pre-compilation)
	//
	#include <iostream>
	#include <fstream>
	#include <sstream>
	#include <string>
	#include <xlocale>
	#include <limits>
	#include <memory>
	#include <exception>

	// Define fixed-size types
	//
	typedef unsigned long g_uint32_t ;
	typedef unsigned short g_uint16_t ;
	typedef long g_int32_t ;
	typedef short g_int16_t ;

	// Define short-name types
	//
	typedef unsigned char uchar_t ;

	// Define missing standard types
	//
	#if defined( G_WINDOWS )
		typedef int ssize_t ;
	#endif

	// STL portability macros
	//
	#if 1
		#define GAllocator(T)
		#define GLessAllocator(T1,T2)
	#else
		#define GAllocator(T) ,std::allocator<T>
		#define GLessAllocator(T1,T2) ,std::allocator<T1>,std::less<T2>
	#endif

	// Modify compiler error handling
	//
	#if G_COMPILER_IS_MICROSOFT
		#pragma warning( disable : 4100 ) // unused formal parameter
		#pragma warning( disable : 4355 ) // 'this' in initialiser list
		#pragma warning( disable : 4511 ) // cannot create default copy ctor
		#pragma warning( disable : 4512 ) // cannot create default op=()
		#pragma warning( disable : 4786 ) // truncation in debug info
		#pragma warning( disable : 4275 ) // dll-interface stuff in <complex>
	#endif

#endif

