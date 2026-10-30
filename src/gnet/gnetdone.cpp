//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gnetdone.cpp
///

#include "gdef.h"
#include "gnetdone.h"

GNet::Done::Done() :
	std::runtime_error("done")
{
}

