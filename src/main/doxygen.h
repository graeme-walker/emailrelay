/*

Copyright (C) 2001-2003 Graeme Walker <graeme_walker@users.sourceforge.net>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/
/* \htmlonly */

/*! \mainpage E-MailRelay Source code

This documentation has been generated by doxygen from the E-MailRelay's
source code. The <a href="namespaces.html">Namespace List</a> is a good starting point
for browsing -- the detailed description section towards the end of each namespace
page gives a list of the namespace's key classes.

The E-MailRelay <a href="../developer.html">developer's guide</a> also gives an overview
of the code structure, including simple class diagrams for the
<a href="../gnet-classes.png">GNet</a> and
<a href="../gsmtp-classes.png">GSmtp</a> namespaces.

*/

/*! \namespace Main
\short
Application-level classes.

The Main namespace contains application-level classes for
the E-MailRelay process.

Key classes are:
- Run
- CommandLine
- Configuration

 */

/*! \namespace GSmtp
\short
SMTP and message-store classes.

The GSmtp namespace contains classes relating to the SMTP
protocol and to e-mail storage.

Key classes are:
- MessageStore
- ClientProtocol
- Client
- ServerProtocol
- Server

 */

/*! \namespace GNet
\short
Network classes.

The GNet namespace contains network interface classes
based on the Berkley socket and WinSock system APIs.

Key classes are:
- EventHandler
- EventLoop
- Socket
- Address
- Resolver
- Server

*/

/*! \namespace G
\short
Low-level classes.

The G namespace contains low-level classes for file-system abstraction,
date and time representation, string utility functions, logging,
command line parsing etc.

Key classes are:
- Str
- GetOpt
- Path
- File
- LogOutput
- Log
- Process

*/

/* \endhtmlonly */