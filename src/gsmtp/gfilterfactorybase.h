//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gfilterfactorybase.h
///

#ifndef G_SMTP_FILTER_FACTORY_BASE_H
#define G_SMTP_FILTER_FACTORY_BASE_H

#include "gdef.h"
#include "gfilter.h"
#include "gstringview.h"
#include "gfilestore.h"
#include "geventstate.h"
#include "gexception.h"
#include <string>
#include <utility>
#include <memory>

namespace GSmtp
{
	class FilterFactoryBase ;
}

//| \class GSmtp::FilterFactoryBase
/// A factory interface for making GSmtp::Filter message processors.
///
class GSmtp::FilterFactoryBase
{
public:
	struct Spec /// Filter specification tuple for GSmtp::FilterFactoryBase::newFilter().
	{
		Spec() ;
		Spec( std::string_view , std::string_view ) ;
		Spec & operator+=( const Spec & ) ;
		std::string first ; // "exit", "file", "net", "spam", "chain", empty on error
		std::string second ; // reason on error, or eg. "/bin/a" if "file", eg. "file:/bin/a,file:/bin/b" if "chain"
	} ;

	virtual std::unique_ptr<Filter> newFilter( GNet::EventState ,
		Filter::Type , const Filter::Config & , const Spec & spec ) = 0 ;
			///< Returns a Filter on the heap. Optionally throws if
			///< an invalid or unsupported filter specification.

	virtual ~FilterFactoryBase() = default ;
		///< Destructor.
} ;

#endif
