//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gsignalsafe.h
///

#ifndef G_SIGNAL_SAFE_H
#define G_SIGNAL_SAFE_H

#include "gdef.h"

namespace G
{
	class SignalSafe ;
}

//| \class G::SignalSafe
/// An empty structure that is used to indicate a signal-safe, reentrant implementation.
/// Any function with SignalSafe in its signature should be kept small and simple,
/// with no exceptions, no logging and no blocking system calls.
///
class G::SignalSafe
{
} ;

#endif
