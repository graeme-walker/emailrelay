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
// poke.c
// 

// This is a small program that connects to the
// specified port on the local machine, sends
// a fixed string, and prints out the first
// bit of what it gets sent back.
//
// Its purpose is to provide a low-overhead
// mechanism for stimulating the mail-relay
// daemon to send its queued-up messages to
// the remote smtp server.
//
// usage: poke [<port> [<send-string>]]
//

#if defined(G_WINDOWS) || defined(G_WIN32) || defined(_WIN32) || defined(WIN32)
#include <windows.h>
#include <io.h>
#define write _write
#define STDOUT_FILENO 1
#else
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif
#include <stdlib.h>
#include <string.h>

int main( int argc , char * argv [] )
{
	const char * const host = "127.0.0.1" ;
	unsigned short port = 10025U ;
	char buffer[160U] = "FLUSH" ;
	struct sockaddr_in address ;
	int fd , rc ;

	/* parse the command line -- port number */
	if( argc > 1 ) 
	{
		port = atoi(argv[1]) ;
	}

	/* parse the command line -- send string */
	if( argc > 2 )
	{
		buffer[0] = '\0' ;
		strncat( buffer , argv[2] , sizeof(buffer)-5U ) ;
	}
	strcat( buffer , "\015\012" ) ;

	/* open the socket */
	fd = socket( AF_INET , SOCK_STREAM , 0 ) ;
	if( fd < 0 ) 
		return EXIT_FAILURE ;

	/* prepare the address */
	memset( &address , 0 , sizeof(address) ) ;
	address.sin_family = AF_INET ;
	address.sin_port = htons( port ) ;
	address.sin_addr.s_addr = inet_addr( host ) ;

	/* connect */
	rc = connect( fd , (const struct sockaddr*)&address , sizeof(address) ) ;
	if( rc < 0 ) 
		return EXIT_FAILURE ;

	/* send the string */
	rc = send( fd , buffer , strlen(buffer) , 0 ) ;
	if( rc != (int)strlen(buffer) ) 
		return EXIT_FAILURE ;

	/* read the reply */
	rc = recv( fd , buffer , sizeof(buffer)-1U , 0 ) ;
	if( rc <= 0 ) 
		return EXIT_FAILURE ;

	/* print the reply */
	write( STDOUT_FILENO , buffer , rc ) ;
	buffer[0U] = '\n' ;
	buffer[1U] = '\0' ;
	write( STDOUT_FILENO , buffer , strlen(buffer) ) ;

	return EXIT_SUCCESS ;
}

