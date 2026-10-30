//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gidentity.h
///

#ifndef G_IDENTITY_H
#define G_IDENTITY_H

#include "gdef.h"
#include "gexception.h"
#include "gstringview.h"
#include "gsignalsafe.h"
#include <string>
#include <iostream>
#include <utility>
#include <new>

namespace G
{
	class Identity ;
	class File ;
}

//| \class G::Identity
/// A combination of user-id and group-id, with a very low-level interface
/// to the get/set/e/uid/gid functions. Uses getpwnam() to do username
/// lookups.
/// \see G::Process, G::Root
///
class G::Identity
{
public:
	G_EXCEPTION( NoSuchUser , tx("no such user") )
	G_EXCEPTION( NoSuchGroup , tx("no such group") )
	G_EXCEPTION( Error , tx("cannot read user database") )
	struct FromFile
	{
		friend class G::File ;
		private: FromFile() = default ;
	} ;

	explicit Identity( const std::string & username ,
		const std::string & group_name_override = {} ) ;
			///< Constructor for the named identity.
			///< Throws NoSuchUser on error.

	Identity( FromFile , uid_t , gid_t ) ;
		///< Constructor taking Unix uid and gid, used by G::File.
		///< Initialises with invalid() on Windows.

	static Identity effective() noexcept ;
		///< Returns the current effective identity.

	static Identity real() noexcept ;
		///< Returns the calling process's real identity.

	static Identity root() noexcept ;
		///< Returns the superuser identity.

	static Identity invalid() noexcept ;
		///< Returns an invalid identity.

	static Identity invalid( SignalSafe ) noexcept ;
		///< Returns an invalid identity, with a
		///< signal-safe guarantee.

	bool isRoot() const noexcept ;
		///< Returns true if the userid is zero.

	std::string str() const ;
		///< Returns a string representation.

	uid_t userid() const noexcept ;
		///< Returns the user part (Unix) or the RID (Windows).

	gid_t groupid() const noexcept ;
		///< Returns the group part (Unix) or -1 (Windows).

	std::string sid() const ;
		///< Returns the SID (Windows).

	bool operator==( const Identity & ) const noexcept ;
		///< Comparison operator.

	bool operator!=( const Identity & ) const noexcept ;
		///< Comparison operator.

	static std::pair<Identity,std::string> lookup( std::string_view user ) ;
		///< Does a username lookup returning the identity and the
		///< canonical name. Throws if no such user or on error.

	static std::pair<Identity,std::string> lookup( std::string_view user , std::nothrow_t ) ;
		///< Does a username lookup returning the identity and the
		///< canonical name. Returns with Identitiy::invalid() if
		///< no such user. Throws on error.

	static gid_t lookupGroup( const std::string & group ) ;
		///< Does a groupname lookup. Returns -1 if no
		///< such group. Throws on error.

	bool match( std::pair<int,int> uid_range ) const ;
		///< Returns true if this identity's user-id value (Unix)
		///< or RID (Windows) is in the given inclusive range.

private:
	Identity() noexcept ;
	explicit Identity( SignalSafe ) noexcept ;
	Identity( uid_t , gid_t ) ;
	Identity( uid_t , gid_t , const std::string & ) ;

private:
	uid_t m_uid ;
	gid_t m_gid ;
	std::string m_sid ; // windows
} ;

namespace G
{
	inline
	std::ostream & operator<<( std::ostream & stream , const Identity & identity )
	{
		return stream << identity.str() ;
	}
}

#endif
