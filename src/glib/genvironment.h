//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file genvironment.h
///

#ifndef G_ENVIRONMENT_H
#define G_ENVIRONMENT_H

#include "gdef.h"
#include "gexception.h"
#include "gstringview.h"
#include "gpath.h"
#include <string>
#include <vector>
#include <map>

namespace G
{
	class Environment ;
}

//| \class G::Environment
/// Holds a set of environment variables and also provides static methods
/// to wrap getenv() and putenv().
///
class G::Environment
{
public:
	G_EXCEPTION( Error , tx("invalid environment variable") )

	static std::string get( const std::string & name , const std::string & default_ ) ;
		///< Returns the environment variable value or the given default.

	static G::Path getPath( const std::string & name , const G::Path & = {} ) ;
		///< Returns the environment variable value as a G::Path object.

	static void put( const std::string & name , const std::string & value ) ;
		///< Sets the environment variable value.

	static Environment minimal( bool sbin = false ) ;
		///< Returns a minimal, safe set of environment variables.

	static Environment inherit() ;
		///< Returns an empty() environment, as if default constructed.
		///< This is syntactic sugar for the G::NewProcess interface.

	explicit Environment( const std::map<std::string,std::string> & ) ;
		///< Constructor from a map.

	bool contains( const std::string & name ) const ;
		///< Returns true if the given variable is in this set.

	std::string value( const std::string & name , const std::string & default_ = {} ) const ;
		///< Returns the value of the given variable in this set.

	bool add( std::string_view key , std::string_view value ) ;
		///< Adds an environment variable. Returns false if invalid.

	std::string block() const ;
		///< Returns a contiguous block of memory containing the
		///< null-terminated strings with an extra zero byte
		///< at the end.

	std::wstring block( std::wstring (*)(std::string_view) ) const ;
		///< Returns a contiguous block of memory containing the
		///< null-terminated strings with an extra zero character
		///< at the end.

	bool empty() const noexcept ;
		///< Returns true if empty.

	static std::vector<char*> array( const std::string & block ) ;
		///< Returns a pointer array pointing into the given
		///< block(), with const-casts.

	static std::vector<char*> array( std::string && envblock ) = delete ;
		///< Deleted overload.

	std::map<std::string,std::string> map() const ;
		///< Returns the environment as a map.

private:
	using Map = std::map<std::string,std::string> ;
	using List = std::vector<std::string> ;

private:
	static void sanitise( Map & ) ;

private:
	Map m_map ;
} ;

inline
std::map<std::string,std::string> G::Environment::map() const
{
	return m_map ;
}

inline
bool G::Environment::empty() const noexcept
{
	return m_map.empty() ;
}

inline
G::Environment G::Environment::inherit()
{
	return Environment( {} ) ;
}

#endif
