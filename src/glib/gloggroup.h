//
// gloggroup.h
//

#ifndef G_LOG_GROUP_H
#define G_LOG_GROUP_H

#include "gdef.h"
#include <cstddef>

namespace G
{
	namespace LogGroupImp
	{
		constexpr bool end( char c ) noexcept
		{
			return c == '\0' || c == ',' ;
		}
		constexpr bool strequal( const char * p1 , const char * p2 ) noexcept
		{
			return ( end(*p1) && end(*p2) ) ? true : ( *p1 == *p2 ? strequal( p1+1 , p2+1 ) : false ) ;
		}
		constexpr int strfind( const char * key , const char * list , int n = 0 , int id = 0 ) noexcept
		{
			return *list ? ( strequal(key,list) ? id : strfind(key,list+1,n+1,*list==','?(id+1):id) ) : -1 ;
		}
		constexpr std::size_t strcommas( const char * p , std::size_t n = 0U ) noexcept
		{
			return *p ? strcommas(p+1,n+(*p==','?1U:0U)) : n ;
		}
	}
	namespace LogGroup
	{
		// NOLINTBEGIN(*-avoid-c-arrays)
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
			"tls,"
			"dnsbl"
		;
		// NOLINTEND(*-avoid-c-arrays)
		constexpr std::size_t list_size = (LogGroupImp::strcommas(list)+1U) ;
	}
	struct LogGroupId
	{
		int value ;
	} ;
	namespace LogGroupLiterals
	{
		// compile-time mapping from string to id with c++20 compile-time validation: "foo"_id.value
		#if __cplusplus >= 202000L
		consteval LogGroupId operator"" _id( const char * name , std::size_t )
		{
			using namespace LogGroupImp ;
			if( strfind( name , LogGroup::list ) == -1 ) throw "invalid log-group name" ;
			return { strfind( name , LogGroup::list ) } ;
		}
		#else
		constexpr LogGroupId operator"" _id( const char * name , std::size_t )
		{
			using namespace LogGroupImp ;
			return { strfind( name , LogGroup::list ) } ;
		}
		#endif
	}
}
using namespace G::LogGroupLiterals ;

#endif
