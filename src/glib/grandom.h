//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file grandom.h
///

#ifndef G_RANDOM_H
#define G_RANDOM_H

#include "gdef.h"
#include <random>

namespace G
{
	namespace Random /// An enclosing namespace for G::Random::rand().
	{
		unsigned int rand( unsigned int start = 0U , unsigned int end = 32767 ) ;
			///< Returns a random value, uniformly distributed over the
			///< given range (including 'start' and 'end'), and automatically
			///< seeded on first use.
	}
}

#endif
