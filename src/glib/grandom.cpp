//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file grandom.cpp
///

#include "gdef.h"
#include "grandom.h"
#include "gprocess.h"
#include <ctime>
#include <chrono>
#include <random>

unsigned int G::Random::rand( unsigned int start , unsigned int end )
{
	static std::default_random_engine e ; // NOLINT cert-msc32-c

	static bool seeded = false ;
	if( !seeded )
	{
		#if defined(G_WINDOWS)
			std::random_device r ;
		#else
			std::random_device r( "/dev/urandom" ) ;
		#endif

		using seed_t = std::random_device::result_type ;
		seed_t seed_1 = 0U ;
		try { seed_1 = r() ; } catch( std::exception & ) {}

		auto tp = std::chrono::high_resolution_clock::now() ;
		auto seed_2 = static_cast<seed_t>( tp.time_since_epoch().count() ) ;

		seed_t seed_3 = G::Process::Id().seed<seed_t>() ;

		std::seed_seq seq{ seed_1 , seed_2 , seed_3 } ;
		e.seed( seq ) ;
		seeded = true ;
	}

	std::uniform_int_distribution<unsigned int> dist( start , end ) ;
	return dist( e ) ;
}

