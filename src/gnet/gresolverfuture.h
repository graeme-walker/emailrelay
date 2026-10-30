//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gresolverfuture.h
///

#ifndef G_NET_RESOLVER_FUTURE_H
#define G_NET_RESOLVER_FUTURE_H

#include "gdef.h"
#include "gaddress.h"
#include "gresolver.h"
#include <utility>
#include <vector>
#include <string>

namespace GNet
{
	class ResolverFuture ;
}

//| \class GNet::ResolverFuture
/// A 'future' shared-state class for asynchronous name resolution that
/// holds parameters and results of a call to getaddrinfo(), as performed
/// by the run() method.
///
/// The run() method can be called from a worker thread and the results
/// collected by the main thread using get() once the worker thread has
/// signalled that it has finished. The signalling mechanism is outside
/// the scope of this class (see GNet::FutureEvent).
///
/// Eg:
/// \code
///
/// ResolverFuture f( "example.com" , "smtp" , AF_INET , false ) ;
/// std::thread t( &ResolverFuture::run , f ) ;
/// ...
/// t.join() ;
/// Address a = f.get().first ;
/// if( f.error() ) throw std::runtime_error( f.reason() ) ;
/// \endcode
///
class GNet::ResolverFuture
{
public:
	struct Result /// Result structure for GNet::ResolverFuture::get().
	{
		Address address ;
		std::string canonicalName ; // if requested
	} ;
	using List = std::vector<Address> ;

	ResolverFuture( const std::string & host , const std::string & service ,
		int family , const Resolver::Config & ) ;
			///< Constructor for resolving the given host and service names.

	~ResolverFuture() ;
		///< Destructor.

	ResolverFuture & run() noexcept ;
		///< Does the synchronous name resolution and stores the result.
		///< Returns *this.

	Result get() ;
		///< Returns the resolved address after run() has completed.
		///< Returns a default address if an error().

	void get( List & ) ;
		///< Returns by reference the resolved addresses after run() has
		///< completed by appending to the given list. Appends nothing
		///< if an error().

	bool error() const ;
		///< Returns true if name resolution failed or no suitable
		///< address was returned. Use after get().

	std::string reason() const ;
		///< Returns the reason for the error().
		///< Precondition: error()

public:
	ResolverFuture( const ResolverFuture & ) = delete ;
	ResolverFuture( ResolverFuture && ) = delete ;
	ResolverFuture & operator=( const ResolverFuture & ) = delete ;
	ResolverFuture & operator=( ResolverFuture && ) = delete ;

private:
	static std::string encode( const std::string & , bool ) ;
	std::string failure() const ;
	bool fetch( List & ) const ;
	bool fetch( Result & ) const ;
	bool failed() const ;
	std::string none() const ;
	std::string ipvx() const ;

private:
	Resolver::Config m_config ;
	bool m_numeric_service ;
	std::string m_host ;
	const char * m_host_p ;
	std::string m_service ;
	const char * m_service_p ;
	int m_family ;
	struct addrinfo m_ai_hint {} ;
	int m_rc {0} ;
	struct addrinfo * m_ai {nullptr} ;
	std::string m_reason ;
} ;

#endif
