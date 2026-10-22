//
// gmodule.h
//

#ifndef G_MODULE__H
#define G_MODULE__H

#include "gdef.h"

namespace G
{
	namespace ModuleImp
	{
		static constexpr const char* list [] {
			"" ,
			"io" ,
			"connection" ,
			"auth" ,
			"spam" ,
			"store" ,
			"smtpclient" ,
			"smtpserver" ,
			nullptr ,
		} ;
		constexpr bool strequal( const char * p1 , const char * p2 ) noexcept
		{
			return ( *p1 == '\0' && *p2 == '\0' ) ? true : ( *p1 == *p2 ? strequal( p1+1 , p2+1 ) : false ) ;
		}
		#if __cplusplus < 201700L
		constexpr int id_r( const char * name , std::size_t i )
		{
			return list[i] != nullptr ? ( strequal(name,list[i]) ? i : id_r(name,i+1U) ) : -1 ;
		}
		#else
		template<char... Name>
		struct string_literal
		{
			inline static constexpr char value[] = { Name... , '\0' } ;
		} ;
		template<class S>
		constexpr int id_imp()
		{
			constexpr const char * s = S::value ;
			int i = 0 ;
			for( const char * const * p = list ; *p ; ++p , i++ )
			{
				if( strequal( list[i] , s ) )
					return i;
			}
			return -1 ;
		}
		template<typename S, int Id = id_imp<S>()>
		constexpr int id()
		{
			static_assert( Id >= 0 , "invalid string literal" ) ;
			return Id ;
		}
		#endif
	}
	struct ModuleId
	{
		int value ;
	} ;
	namespace ModuleLiterals
	{
		// compile-time mapping from string to id with c++17 compile-time
		// validation: "foo"_id.value
		//
		#if __cplusplus < 201700L
		constexpr ModuleId operator"" _id( const char * name , std::size_t )
		{
			using namespace ModuleImp ;
			return { id_r( name , 0U ) } ;
		}
		#else
		template<typename T, T... Name>
		constexpr ModuleId operator""_id()
		{
			using namespace ModuleImp ;
			return { id<string_literal<Name...>>() } ;
		}
		#endif
	}
}
using namespace G::ModuleLiterals ;

#endif
