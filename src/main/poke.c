// 
// Copyright (C) 2001 Graeme Walker <graeme_walker@users.sourceforge.net>
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

#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>

int main( int argc , char * argv [] )
{
	const char * const host = "127.0.0.1" ;
	unsigned short port = 10025U ;
	char buffer[160U] = "FLUSH" ;
	struct sockaddr_in address ;
	int fd , rc ;

	if( argc > 1 ) 
		port = atoi(argv[1]) ;

	if( argc > 2 )
	{
		buffer[0] = '\0' ;
		strncat( buffer , argv[2] , sizeof(buffer)-5U ) ;
	}
	strcat( buffer , "\015\012" ) ;

	fd = socket( AF_INET , SOCK_STREAM , 0 ) ;
	if( fd < 0 ) return EXIT_FAILURE ;
	memset( &address , 0 , sizeof(address) ) ;
	address.sin_family = AF_INET ;
	address.sin_port = htons( port ) ;
	address.sin_addr.s_addr = inet_addr( host ) ;
	rc = connect( fd , (const struct sockaddr*)&address , sizeof(address) ) ;
	if( rc < 0 ) return EXIT_FAILURE ;
	rc = write( fd , buffer , strlen(buffer) ) ;
	if( rc != strlen(buffer) ) return EXIT_FAILURE ;
	rc = read( fd , buffer , sizeof(buffer)-1U) ;
	if( rc <= 0 ) return EXIT_FAILURE ;
	write( STDOUT_FILENO , buffer , rc ) ;
	buffer[0U] = '\n' ;
	buffer[1U] = '\0' ;
	write( STDOUT_FILENO , buffer , strlen(buffer) ) ;
	return EXIT_SUCCESS ;
}

