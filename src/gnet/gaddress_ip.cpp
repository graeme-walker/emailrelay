//
// Copyright (C) 2001-2008 Graeme Walker <graeme_walker@users.sourceforge.net>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// ===
//
// gaddress_ip.cpp
//

#include "gdef.h"
#ifdef USE_IPV6
#include "gaddress_ipv6.cpp"
#else
#include "gaddress_ipv4.cpp"
#endif

/// \file gaddress_ip.cpp
