//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: GPL-3.0-or-later
//
///
/// \file gverifierfactory.cpp
///

#include "gdef.h"
#include "gverifierfactory.h"
#include "ginternalverifier.h"
#include "gexecutableverifier.h"
#include "gnetworkverifier.h"
#include "guserverifier.h"
#include "gfile.h"
#include "gstr.h"
#include "gstringtoken.h"
#include "grange.h"
#include "gexception.h"

GVerifiers::VerifierFactory::VerifierFactory()
= default ;

GVerifiers::VerifierFactory::Spec GVerifiers::VerifierFactory::parse( std::string_view spec_in ,
	const G::Path & base_dir , const G::Path & app_dir , G::StringArray * warnings_p )
{
	std::string_view tail = G::Str::tailView( spec_in , ":" ) ;
	Spec result ;
	if( spec_in.empty() )
	{
		result = makeSpec( "exit" , "0" ) ;
		checkExit( result ) ;
	}
	else if( G::Str::headMatch( spec_in , "exit:" ) )
	{
		result = makeSpec( "exit" , tail ) ;
		checkExit( result ) ;
	}
	else if( G::Str::headMatch( spec_in , "net:" ) )
	{
		result = makeSpec( "net" , tail ) ;
		checkNet( result ) ;
	}
	else if( G::Str::headMatch( spec_in , "account:" ) )
	{
		result = makeSpec( "account" , tail ) ;
		checkRange( result ) ;
	}
	else if( G::Str::headMatch( spec_in , "file:" ) )
	{
		result = makeSpec( "file" , tail ) ;
		fixFile( result , base_dir , app_dir ) ;
		checkFile( result , warnings_p ) ;
	}
	else
	{
		result = makeSpec( "file" , spec_in ) ;
		fixFile( result , base_dir , app_dir ) ;
		checkFile( result , warnings_p ) ;
	}
	return result ;
}

GSmtp::VerifierFactoryBase::Spec GVerifiers::VerifierFactory::makeSpec( std::string_view first , 
	std::string_view second )
{
	std::string s = G::sv_to_string( second ) ;
	return { first , G::LogOutput::noVerbosity(s) , G::LogOutput::parseVerbosity(s) } ;
}

std::unique_ptr<GSmtp::Verifier> GVerifiers::VerifierFactory::newVerifier( GNet::EventState es ,
	GSmtp::Verifier::Config verifier_config , const Spec & spec )
{
	verifier_config.verbosity = spec.verbosity ;
	if( spec.first == "exit" )
	{
		return std::make_unique<InternalVerifier>() ;
	}
	else if( spec.first == "net" )
	{
		return std::make_unique<NetworkVerifier>( es , verifier_config , spec.second ) ;
	}
	else if( spec.first == "account" )
	{
		return std::make_unique<UserVerifier>( es , verifier_config , spec.second ) ;
	}
	else if( spec.first == "file" )
	{
		return std::make_unique<ExecutableVerifier>( es , verifier_config , spec.second ) ;
	}
	throw G::Exception( "invalid verifier" , spec.second ) ;
}

void GVerifiers::VerifierFactory::checkExit( Spec & result )
{
	if( !G::Str::isUInt(result.second) )
	{
		result.first.clear() ;
		result.second = "not a numeric exit code: " + G::Str::printable(result.second) ;
	}
}

void GVerifiers::VerifierFactory::checkNet( Spec & result )
{
	try
	{
		GNet::Location::nosocks( result.second ) ;
	}
	catch( std::exception & e )
	{
		result.first.clear() ;
		result.second = e.what() ;
	}
}

void GVerifiers::VerifierFactory::checkRange( Spec & result )
{
	try
	{
		std::string_view spec_view( result.second ) ;
		for( G::StringTokenView t( spec_view , ";" , 1U ) ; t ; ++t )
		{
			if( !t().empty() && G::Str::isNumeric(t().substr(0U,1U)) )
				G::Range::check( t() ) ;
		}
	}
	catch( std::exception & e )
	{
		result.first.clear() ;
		result.second = e.what() ;
	}
}

void GVerifiers::VerifierFactory::fixFile( Spec & result , const G::Path & base_dir , const G::Path & app_dir )
{
	if( !app_dir.empty() && result.second.find("@app") == 0U )
		G::Str::replace( result.second , "@app" , app_dir.str() ) ;
	else if( !base_dir.empty() && G::Path(result.second).isRelative() )
		result.second = (base_dir/result.second).str() ;
}

void GVerifiers::VerifierFactory::checkFile( Spec & result , G::StringArray * warnings_p )
{
	if( result.second.empty() )
	{
		result.first.clear() ;
		result.second = "empty file path" ;
	}
	else if( warnings_p && !G::File::exists(result.second) )
	{
		warnings_p->push_back( std::string("verifier program does not exist: ").append(result.second) ) ;
	}
	else if( warnings_p && G::File::isDirectory(result.second,std::nothrow) )
	{
		warnings_p->push_back( std::string("invalid program: ").append(result.second) ) ;
	}
}

