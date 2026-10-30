//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file news.cpp
///

#include "news.h"
#include "gdatetime.h"

std::string Main::News::text( const std::string & eol )
{
return std::string() +
"Free software is a matter of liberty, not price. To understand " +
"the concept, you should think of \"free\" as in \"free speech,\" " +
"not as in \"free beer\". Windows is not free, even if you did not pay for it, " +
"because it limits your freedoms; check out the Free Software Foundation " +
"website http://www.fsf.org for more information about software freedom." +
eol
;
}
