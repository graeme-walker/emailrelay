//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gsmtpserversend.h
///

#ifndef G_SMTP_SERVER_SEND_H
#define G_SMTP_SERVER_SEND_H

#include "gdef.h"
#include "gsmtpserversender.h"
#include "gstringarray.h"
#include <string>
#include <sstream>

namespace GSmtp
{
	class ServerSend ;
}

//| \class GSmtp::ServerSend
/// A simple mix-in class for GSmtp::ServerProtocol that sends protocol
/// responses via a GSmtp::ServerSender.
///
class GSmtp::ServerSend
{
public:
	explicit ServerSend( ServerSender * ) ;
		///< Constructor.

	void setSender( ServerSender * ) ;
		///< Sets the sender interface pointer.

	virtual bool sendFlush() const = 0 ;
		///< Returns a 'flush' value for GSmtp::ServerSender::protocolSend().

public:
	virtual ~ServerSend() = default ;
	ServerSend( const ServerSend & ) = delete ;
	ServerSend( ServerSend && ) = delete ;
	ServerSend & operator=( const ServerSend & ) = delete ;
	ServerSend & operator=( ServerSend && ) = delete ;

protected:
	struct Advertise /// Configuration for the EHLO response.
	{
		std::string hello ;
		std::size_t max_size {0U} ;
		G::StringArray mechanisms ;
		bool starttls {false} ;
		bool vrfy {false} ;
		bool chunking {false} ;
		bool binarymime {false} ;
		bool pipelining {false} ;
		bool smtputf8 {false} ;
	} ;
	void send( const char * ) ;
	void send( std::string , bool = false ) ;
	void send( const std::ostringstream & ) ;
	void sendReadyForTls() ;
	void sendInsecureAuth( bool = false ) ;
	void sendEncryptionRequired( bool = false ) ;
	void sendBadMechanism( const std::string & = {} ) ;
	void sendBadFrom( const std::string & ) ;
	void sendTooBig() ;
	void sendChallenge( const std::string & ) ;
	void sendBadTo( const std::string & , const std::string & , bool ) ;
	void sendBadDataOutOfSequence() ;
	void sendOutOfSequence() ;
	void sendGreeting( const std::string & , bool ) ;
	void sendQuitOk() ;
	void sendUnrecognised( const std::string & ) ;
	void sendNotImplemented() ;
	void sendHeloReply() ;
	void sendEhloReply( const Advertise & ) ;
	void sendRsetReply() ;
	void sendMailReply( const std::string & from ) ;
	void sendRcptReply( const std::string & to , bool local ) ;
	void sendDataReply() ;
	void sendCompletionReply( bool ok , int , const std::string & ) ;
	void sendFailed() ;
	void sendInvalidArgument() ;
	void sendAuthenticationCancelled() ;
	void sendAuthRequired( bool = false ) ;
	void sendDisabled() ;
	void sendNoRecipients() ;
	void sendMissingParameter() ;
	void sendVerified( const std::string & ) ;
	void sendCannotVerify() ;
	void sendNotVerified( const std::string & , bool ) ;
	void sendWillAccept( const std::string & ) ;
	void sendAuthDone( bool ok ) ;
	void sendOk( const std::string & ) ;
	void sendOk() ;

private:
	ServerSender * m_sender ;
} ;

#endif
