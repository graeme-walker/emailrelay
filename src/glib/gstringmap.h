//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gstringmap.h
///

#ifndef G_STRING_MAP_H
#define G_STRING_MAP_H

#include "gdef.h"
#include <string>
#include <map>

namespace G
{
	using StringMap = std::map<std::string,std::string> ; ///< A std::map of std::strings.
}

#endif
