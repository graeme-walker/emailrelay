//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file glimits.h
///

#ifndef G_LIMITS_H
#define G_LIMITS_H

#include "gdef.h"

namespace G
{
	enum class Scale
	{
		Normal ,
		Small
	} ;
	#ifdef G_SMALL
	template <Scale N = Scale::Small> struct Limits ;
	#else
	template <Scale N = Scale::Normal> struct Limits ;
	#endif
	template <> struct Limits<Scale::Normal> ;
	template <> struct Limits<Scale::Small> ;
}

//| \class G::Limits
/// A set of compile-time buffer sizes. Intended to be used to
/// reduce memory requirements in embedded environments.
///
template <G::Scale N>
struct G::Limits
{
} ;

template <>
struct G::Limits<G::Scale::Normal> /// Normal specialisation of G::Limits.
{
	static constexpr bool is_small = false ;
	static constexpr int log = 1000 ; // log line limit
	static constexpr int path_buffer = 1024 ; // getcwd() first-attempt buffer size
	static constexpr int file_buffer = 8192 ; // read() buffer size for file copying (BUFSIZ)
	static constexpr int net_buffer = 20000 ; // read() buffer size for network reads (>=16k is best for TLS)
	static constexpr int net_listen_queue = 31 ; // listen(2) backlog parameter (cf. apache 511)
	static constexpr int net_file_limit = 200000000 ; // DoS limit reading a file from the network
	Limits() = delete ;
} ;

template <>
struct G::Limits<G::Scale::Small> /// Small-memory specialisation of G::Limits.
{
	static constexpr bool is_small = true ;
	static constexpr int log = 120 ;
	static constexpr int path_buffer = 64 ;
	static constexpr int file_buffer = 4096 ;
	static constexpr int net_buffer = 4096 ;
	static constexpr int net_listen_queue = 3 ;
	static constexpr int net_file_limit = 10000000 ;
	Limits() = delete ;
} ;

#endif
