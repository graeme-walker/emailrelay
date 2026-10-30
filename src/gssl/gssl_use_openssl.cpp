//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gssl_use_openssl.cpp
///

#include "gdef.h"
#include "gssl.h"
#include "gssl_openssl.h"

std::unique_ptr<GSsl::LibraryImpBase> GSsl::Library::newLibraryImp( G::StringArray & library_config , Library::LogFn log_fn , bool verbose )
{
	return std::make_unique<OpenSSL::LibraryImp>( library_config , log_fn , verbose ) ; // up-cast
}

std::string GSsl::Library::credit( const std::string & prefix , const std::string & eol , const std::string & eot )
{
	return OpenSSL::LibraryImp::credit( prefix , eol , eot ) ;
}

std::string GSsl::Library::ids()
{
	return OpenSSL::LibraryImp::sid() ;
}

