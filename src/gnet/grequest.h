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
// grequest.h
//

#ifndef G_REQUEST_H
#define G_REQUEST_H

#include "gdef.h"
#include "gnet.h"
#include "gaddress.h"
#include "gwindow.h"

namespace GNet
{
	class Request ;
	class HostRequest ;
	class ServiceRequest ;
} ;

// Class: GNet::Request
// Description: A base class for making
// asynchronous DNS requests.
//
class GNet::Request
{
protected:
	enum { magic = 968 } ;
	int m_magic ;
	int m_error ;
	HANDLE m_handle ;
	char m_buffer[MAXGETHOSTSTRUCT] ;
	bool m_host ;
	bool m_done ;
	bool m_numeric ;
	GNet::Address m_address ;

protected:
	explicit Request( bool host ) ;
		// Constructor. Derived class constructors
		// should issue the appropriate request.

public:
	virtual ~Request() ;
		// Virtual destructor. Cancels any
		// outstanding request.

	bool valid() const ;
		// Returns true if the constructor
		// initiated a request properly.

	std::string reason() const ;
		// Returns the failure reason if
		// valid() or onMessage() returned
		// false.

	bool onMessage( WPARAM wparam , LPARAM lparam ) ;
		// To be called when the request has
		// been completed. Returns false
		// on error.

private:
	Request( const Request & ) ;
	void operator=( const Request & ) ;
	static const char *reason( bool host , int error ) ;
} ;

// Class: GNet::HostRequest
// Description: A derivation of GNet::Request used for hostname lookup requests.
//
class GNet::HostRequest : public Request
{
public:
	HostRequest( std::string host_name , HWND hwnd , unsigned msg ) ;
	Address result() const ; // zero port

private:
	bool numeric( std::string s , Address & address ) ;
	HostRequest( const HostRequest & ) ;
	void operator=( const HostRequest & ) ;
} ;

// Class: GNet::ServiceRequest
// Description: A derivation of GNet::Request used for service (port) lookup requests.
//
class GNet::ServiceRequest : public Request
{
public:
	ServiceRequest( std::string service_name , bool udp , HWND hwnd , unsigned msg ) ;
	Address result() const ; // zero host address

private:
	static const char * protocol( bool udp ) ;
	bool numeric( std::string s , Address & address ) ;
} ;

#endif

