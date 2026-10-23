//
// glogmodule.h
//

#ifndef G_LOG_MODULE_H
#define G_LOG_MODULE_H

#include "gdef.h"

namespace G
{
	namespace LogModuleImp
	{
		inline constexpr bool end( char c ) noexcept
		{
			return c == '\0' || c == ',' ;
		}
		inline constexpr bool strequal( const char * p1 , const char * p2 ) noexcept
		{
			return ( end(*p1) && end(*p2) ) ? true : ( *p1 == *p2 ? strequal( p1+1 , p2+1 ) : false ) ;
		}
		inline constexpr int strfind( const char * key , const char * list , int n = 0 , int id = 0 ) noexcept
		{
			return *list ? ( strequal(key,list) ? id : strfind(key,list+1,n+1,*list==','?(id+1):id) ) : -1 ;
		}
		inline constexpr std::size_t strcommas( const char * p , std::size_t n = 0U ) noexcept
		{
			return *p ? strcommas(p+1,n+(*p==','?1U:0U)) : n ;
		}
	}
	namespace LogModule
	{
		#if __cplusplus >= 201700L
		inline constexpr const char list[] =
		#else
		constexpr const char list[] = // (c++11 not inline, use string-folding compiler options)
		#endif
			"io,"
			"connection,"
			"forwarding,"
			"auth,"
			"spam,"
			"store,"
			"smtp,"
			"pop,"
			"mx,"
			"tls,"
			"dnsbl"
		;
		static_assert( (LogModuleImp::strcommas(list)+1U) <= (sizeof(unsigned)*4U) , "too many module names" ) ;
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
			if( strfind( name , LogModule::list ) == -1 ) throw "invalid module name" ;
			return { strfind( name , LogModule::list ) } ;
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

#endif
