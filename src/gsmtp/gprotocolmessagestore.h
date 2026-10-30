//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gprotocolmessagestore.h
///

#ifndef G_SMTP_PROTOCOL_MESSAGE_STORE_H
#define G_SMTP_PROTOCOL_MESSAGE_STORE_H

#include "gdef.h"
#include "gprotocolmessage.h"
#include "gmessagestore.h"
#include "gnewmessage.h"
#include "gfilter.h"
#include "gslot.h"
#include <string>
#include <memory>

namespace GSmtp
{
	class ProtocolMessageStore ;
}

//| \class GSmtp::ProtocolMessageStore
/// A concrete implementation of the ProtocolMessage interface
/// that stores incoming messages in the message store.
/// \see GSmtp::ProtocolMessageForward
///
class GSmtp::ProtocolMessageStore : public ProtocolMessage
{
public:
	ProtocolMessageStore( GStore::MessageStore & store ,
		std::unique_ptr<Filter> ) ;
			///< Constructor.

	~ProtocolMessageStore() override ;
		///< Destructor.

private: // overrides
	ProtocolMessage::ProcessedSignal & processedSignal() noexcept override ; // GSmtp::ProtocolMessage
	void reset() override ; // GSmtp::ProtocolMessage
	void clear() override ; // GSmtp::ProtocolMessage
	GStore::MessageId setFrom( const std::string & , const FromInfo & ) override ; // GSmtp::ProtocolMessage
	bool addTo( const ToInfo & ) override ; // GSmtp::ProtocolMessage
	void addReceived( const std::string & ) override ; // GSmtp::ProtocolMessage
	GStore::NewMessage::Status addContent( const char * , std::size_t ) override ; // GSmtp::ProtocolMessage
	std::size_t contentSize() const override ; // GSmtp::ProtocolMessage
	std::string from() const override ; // GSmtp::ProtocolMessage
	ProtocolMessage::FromInfo fromInfo() const override ; // GSmtp::ProtocolMessage
	std::string bodyType() const override ; // GSmtp::ProtocolMessage
	void process( const std::string & auth_id , const std::string & peer_socket_address ,
		const std::string & peer_certificate ) override ; // GSmtp::ProtocolMessage

public:
	ProtocolMessageStore( const ProtocolMessageStore & ) = delete ;
	ProtocolMessageStore( ProtocolMessageStore && ) = delete ;
	ProtocolMessageStore & operator=( const ProtocolMessageStore & ) = delete ;
	ProtocolMessageStore & operator=( ProtocolMessageStore && ) = delete ;

private:
	void filterDone( int ) ;

private:
	GStore::MessageStore & m_store ;
	std::unique_ptr<Filter> m_filter ;
	std::unique_ptr<GStore::NewMessage> m_new_msg ;
	std::string m_from ;
	FromInfo m_from_info ;
	ProtocolMessage::ProcessedSignal m_processed_signal ;
} ;

#endif

