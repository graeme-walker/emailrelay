//
// Copyright (C) 2001-2002 Graeme Walker <graeme_walker@users.sourceforge.net>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
// ===
//
// configuration.h
//

#ifndef G_MAIN_CONFIGURATION_H
#define G_MAIN_CONFIGURATION_H

#include "gdef.h"
#include "gsmtp.h"
#include "gpath.h"
#include <string>

namespace Main
{
	class Configuration ;
	class CommandLine ;
} ;

// Class: Main::Configuration
// Description: An interface for returning application configuration
// information. This implementation is minimaly dependent on the
// command line in order to simplify moving to the windows registry
// (for example) in the future.
// See also: CommandLine
//
class Main::Configuration
{
public:
	explicit Configuration( const CommandLine & cl ) ;
		// Constructor. The reference is kept.

	std::string str( const std::string & line_prefix = std::string() ,
		const std::string & eol = std::string("\n") ) const ;
			// Reports the configuration in a multi-line
			// string.

	unsigned int port() const ;
		// Returns the main port number.

	unsigned int adminPort() const ;
		// Returns the admin port number.

	bool closeStderr() const ;
		// Returns true if stderr should be closed.

	bool immediate() const ;
		// Returns true if proxying.

	bool log() const ;
		// Returns true if doing logging.

	bool verbose() const ;
		// Returns true if doing debug-level logging.

	bool syslog() const ;
		// Returns true if generating syslog events.

	bool logTimestamp() const ;
		// Returns true if logging output should be timestamped.

	bool daemon() const ;
		// Returns true if running as a daemon.

	bool doForwarding() const ;
		// Returns true if running as a client.

	bool doServing() const ;
		// Returns true if running as a server.

	bool doAdmin() const ;
		// Returns true if enabling the admin interface.

	bool allowRemoteClients() const ;
		// Returns true if allowing remote clients to connect.

	G::Path spoolDir() const ;
		// Returns the spool directory.

	std::string serverAddress() const ;
		// Returns the downstream server's address string.

	bool usePidFile() const ;
		// Returns true if writing a pid file.

	std::string pidFile() const ;
		// Returns the pid file's path.

	bool useFilter() const ;
		// Returns true if pre-processing.

	std::string filter() const ;
		// Returns the pre-processor's path.

	unsigned int icon() const ;
		// Returns the icon selector.

	unsigned int responseTimeout() const ;
		// Returns the client-side protocol timeout value.

	unsigned int connectionTimeout() const ;
		// Returns the client-side connection timeout value.

	std::string clientSecretsFile() const ;
		// Returns the client-side autentication secrets (password) file.
		// Returns the empty string if none.

	std::string serverSecretsFile() const ;
		// Returns the server-side autentication secrets (password) file.
		// Returns the empty string if none.

	std::string fqdn() const ;
		// Returns the fully-qualified-domain-name override.

	std::string nobody() const ;
		// Returns the name of an unprivileged user. This is only
		// used if running with a real user-id of root.

private:
	const CommandLine & m_cl ;

private:
	static std::string yn( bool ) ;
} ;

#endif

