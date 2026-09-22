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

	bool hasBad() const ;
		///< Returns true if one or more inputs are invalid.

	std::string badName() const ;
		///< Returns the first invalid input.

	bool hasEmpties() const ;
		///< Returns true if some named interfaces have no addresses.

	std::string logEmpties() const ;
		///< Returns a log-line snippet for hasEmpties().

	bool noUpdates() const ;
		///< Returns true if some inputs are interfaces but
		///< GNet::Interfaces is not active().

	const std::vector<int> & fds() const ;
		///< Exposes the list of fd inputs.

	const std::vector<Address> & fixed() const ;
		///< Exposes the list of address inputs.

	const std::vector<Address> & dynamic() const ;
		///< Exposes the list of interface addresses.

private:
	bool empty() const ;
	void addWildcards( const std::vector<unsigned> & ) ;
	static int parseFd( const std::string & ) ;
	static bool isAddress( const std::string & , unsigned int ) ;
	static Address address( const std::string & , unsigned int ) ;
	static int af( const std::string & ) ;
	static std::string basename( const std::string & ) ;
	static bool isBad( const std::string & ) ;

private:
	std::string m_bad ;
	G::StringArray m_empties ;
	G::StringArray m_used ;
	std::vector<Address> m_fixed ;
	std::vector<Address> m_dynamic ;
	std::vector<int> m_fds ;
} ;

#endif
