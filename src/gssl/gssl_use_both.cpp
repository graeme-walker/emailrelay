//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gssl_use_both.cpp
///

#include "gdef.h"
#include "gssl.h"
#include "gssl_openssl.h"
#include "gssl_mbedtls.h"
#include "gtest.h"

std::unique_ptr<GSsl::LibraryImpBase> GSsl::Library::newLibraryImp( G::StringArray & library_config , Library::LogFn log_fn , bool verbose )
{
	if( LibraryImpBase::consume(library_config,"mbedtls") || G::Test::enabled("ssl-use-mbedtls") )
	{
		return std::make_unique<MbedTls::LibraryImp>( library_config , log_fn , verbose ) ;
	}
	else
	{
		LibraryImpBase::consume( library_config , "openssl" ) ;
		return std::make_unique<OpenSSL::LibraryImp>( library_config , log_fn , verbose ) ;
	}
}

std::string GSsl::Library::credit( const std::string & prefix , const std::string & eol , const std::string & eot )
{
	return
		OpenSSL::LibraryImp::credit( prefix , eol , eol ) +
		MbedTls::LibraryImp::credit( prefix , eol , eot ) ;
}

std::string GSsl::Library::ids()
{
	return OpenSSL::LibraryImp::sid() + ", " + MbedTls::LibraryImp::sid() ;
}

