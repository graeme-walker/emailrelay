//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gstatemachine.cpp
///

#include "gdef.h"
#include "gstatemachine.h"
#include <stdexcept>

void G::StateMachineImp::throwError()
{
	throw Error() ;
}

