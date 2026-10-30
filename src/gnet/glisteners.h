//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file glisteners.h
///

#ifndef G_LISTENERS_H
#define G_LISTENERS_H

#include "gdef.h"
#include "ginterfaces.h"
#include "gaddress.h"
#include "gstringarray.h"
#include "gexception.h"
#include <vector>
#include <string>

namespace GNet
{
	class Listeners ;
}

//| \class GNet::Listeners
/// Represents a set of listening inputs which can be file-descriptor,
/// interface or network address.
///
class GNet::Listeners
{
public:
	G_EXCEPTION( InvalidFd , tx("invalid listening file descriptor number") )

	Listeners( const Interfaces & , const G::StringArray & listener_spec_list ,
		const std::vector<unsigned> & ports ) ;
			///< Constructor. The specification strings can be like "fd#3"
			///< for a file descriptor, "127.0.0.1" for a fixed address,
			///< or "ppp0-ipv4" for an interface. If the specification
			///< list is empty then the two fixed wildcard addresses
			///< are added.

	bool defunct() const ;
		///< Returns true if no inputs and static.

	bool idle() const ;
		///< Returns true if no inputs but some interfaces might come up.

	G::StringArray interfaceNames() const ;
		///< Returns names that were parsed as interfaces and that have addresses.

	G::StringArray emptyNames() const ;
		///< Returns names that were parsed as interfaces but have no addresses.

	G::StringArray invalidNames() const ;
		///< Returns names that were not parsed.

	bool noUpdates() const ;
		///< Returns true if some inputs are interfaces but
		///< GNet::Interfaces is not active().

	const std::vector<int> & fds() const ;
		///< Exposes the list of fd inputs.

	const std::vector<Address> & fixedAddresses() const ;
		///< Exposes the list of fixed address inputs.

	const std::vector<Address> & interfaceAddresses() const ;
		///< Exposes the list of interface addresses.

	static std::string logString( const G::StringArray & names ) ;
		///< A convenience function for logging and diagnostics.

private:
	bool empty() const ;
	void addWildcards( const std::vector<unsigned> & ) ;
	static int parseFd( const std::string & ) ;
	static bool isLocalAddress( const std::string & ) ;
	static bool isAddress( const std::string & ) ;
	static bool isSpecialInterface( const std::string & name ) ;
	static bool isValidInterface( const std::string & name ) ;
	static Address address( const std::string & , unsigned int ) ;
	static int af( const std::string & ) ;
	static std::string basename( const std::string & ) ;

private:
	std::string m_bad ;
	G::StringArray m_invalid_names ;
	G::StringArray m_empty_names ;
	G::StringArray m_interface_names ;
	std::vector<Address> m_fixed_addresses ;
	std::vector<Address> m_interface_addresses ;
	std::vector<int> m_fds ;
} ;

#endif
