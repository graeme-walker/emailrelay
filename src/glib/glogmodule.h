//
// glogmodule.h
//

#ifndef G_LOG_MODULE_H
#define G_LOG_MODULE_H

#include "gdef.h"

namespace G
{
	namespace LogModule
	{
		#if __cplusplus >= 201700L
		inline constexpr const char list[] =
		#else
		constexpr const char list[] = // (not inline, use string-folding compiler options)
		#endif
			"io,"
			"connection,"
			"forwarding,"
			"auth,"
			"spam,"
			"store,"
			"smtpclient,"
			"smtpserver,"
			"pop,"
			"mx,"
			"tls,"
			"dnsbl"
		;
	}
	namespace LogModuleImp
	{
		inline constexpr bool end( char c )
		{
			return c == '\0' || c == ',' ;
		}
		inline constexpr int strlen( const char * p , int n = 0 )
		{
			return end(*p) ? n : strlen(p+1,n+1) ;
		}
		inline constexpr bool strequal( const char * p1 , const char * p2 ) noexcept
		{
			return ( end(*p1) && end(*p2) ) ? true : ( *p1 == *p2 ? strequal( p1+1 , p2+1 ) : false ) ;
		}
		inline constexpr int strfind( const char * key , const char * list , int n = 0 , int id = 0 )
		{
			return *list ? ( strequal(key,list) ? id : strfind(key,list+1,n+1,*list==','?(id+1):id) ) : -1 ;
		}
	}
	struct LogModuleId
	{
		int value ;
	} ;
	namespace LogModuleLiterals
	{
		// compile-time mapping from string to id with c++20 compile-time validation: "foo"_id.value
		#if __cplusplus >= 202000L
		inline consteval LogModuleId operator"" _id( const char * name , std::size_t )
		{
			using namespace LogModuleImp ;
			if( strfind( name , modules ) == -1 ) throw "invalid module name" ;
			return { strfind( name , Module::list ) } ;
		}
		#else
		inline constexpr LogModuleId operator"" _id( const char * name , std::size_t )
		{
			using namespace LogModuleImp ;
			return { strfind( name , LogModule::list ) } ;
		}
		#endif
	}
}
using namespace G::LogModuleLiterals ;

// static_assert( "io"_id.value == 0 , "" ) ;
// static_assert( "connection"_id.value == 1 , "" ) ;
// static_assert( "forwarding"_id.value == 2 , "" ) ;
// static_assert( "auth"_id.value == 3 , "" ) ;
// static_assert( "spam"_id.value == 4 , "" ) ;
// static_assert( "store"_id.value == 5 , "" ) ;
// static_assert( "smtpclient"_id.value == 6 , "" ) ;
// static_assert( "smtpserver"_id.value == 7 , "" ) ;
// static_assert( "sdfklj"_id.value == -1 , "" ) ;

#endif
