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

	// Autoconf stuff (mostly commented out)
	//
	#if HAVE_CONFIG_H
		#if 0
			#include <config.h>

			#if HAVE_UNISTD_H
				#include <sys/types.h>
				#include <unistd.h>
			#endif

			#if HAVE_DIRENT_H
				#include <dirent.h>
			#else
				#define dirent direct
				#if HAVE_SYS_NDIR_H
					#include <sys/ndir.h>
				#endif
				#if HAVE_SYS_DIR_H
					#include <sys/dir.h>
				#endif
				#if HAVE_NDIR_H
					#include <ndir.h>
				#endif
			#endif

			#if TIME_WITH_SYS_TIME
				#include <sys/time.h>
				#include <time.h>
			#else
				#if HAVE_SYS_TIME_H
					#include <sys/time.h>
				#else
					#include <time.h>
				#endif
			#endif
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

	// Define the compiler and its capabilities
	//
	#if defined( _MSC_VER )
		#define G_COMPILER_IS_MICROSOFT 1
		// #define G_COMPILER_HAS_... 0
	#endif
	#if defined( __GNUC__ )
		#define G_COMPILER_IS_GNU 1
		// #define G_COMPILER_HAS_... 1
	#endif

	// Modify compiler error handling for system headers
	//
	#if defined( G_COMPILER_IS_MICROSOFT )
		#pragma warning( disable : 4514 ) // don't reenable
		#pragma warning( push , 3 )
		#pragma warning( disable : 4201 )
		#pragma warning( disable : 4514 ) // again
		#pragma warning( disable : 4663 4018 4146 4018 )
		#pragma warning( disable : 4244 4100 4512 4511 )
	#endif

	// Include main operating-system headers
	//
	#if defined( G_WINDOWS )
		#include <windows.h>
		#include <shellapi.h>
	#else
		#include <unistd.h>
		#include <sys/stat.h>
	#endif

	// Restore complier error handling
	//
	#if defined( G_COMPILER_IS_MICROSOFT )
		#pragma warning( default : 4201 )
		#pragma warning( default : 4663 4018 4146 4018 )
		#pragma warning( default : 4244 4100 4512 4511 )
		#pragma warning( pop )
	#endif

	// Define Windows-style types (under Unix these
	// are only used for unimplemented declarations)
	//
	#if ! defined( G_WINDOWS )
		typedef unsigned char BOOL ;
		typedef unsigned int HWND ;
		typedef unsigned int HINSTANCE ;
		typedef unsigned int HANDLE ;
	#endif

	// Include commonly-used system headers
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

	// Define short-name types
	//
	typedef unsigned char uchar_t ;

	// Define missing standard types
	//
	#if defined( G_WINDOWS )
		typedef int ssize_t ; // (should be in sys/types.h)
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
	#endif

#endif

