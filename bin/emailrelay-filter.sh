#!/bin/sh
# 
# Copyright (C) 2001 Graeme Walker <graeme_walker@users.sourceforge.net>
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later
# version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
# 
# ===
#
# A doxygen filter.
#

awk="gawk"

# PreFilter()
# Removes banner comments (including legalese) from the top of the file.
#
PreFilter()
{
	${awk} ' BEGIN {
		in_start = 1
	}
	{
		if( in_start && !match( $0 , "^//" ) )
			in_start = 0
		if( !in_start )
			print
	} '
}

# SourceFilter()
# Adds doxygen annotation for implementation class comments.
#
SourceFilter()
{
	${awk} ' BEGIN { in_class_comment = 0 }
		{
			start = match( $0 , "// Class:" ) == 1
			end = match( $0 , "class" ) == 1

			if( !in_class_comment && start )
			{
				in_class_comment = 1
				print "/**"
			}
			else if( !in_class_comment )
			{
				print
			}
			else if( end )
			{
				in_class_comment = 0
				print " */"
				print
			}
			else
			{
				sub( "^// Description: " , "" )
				sub( "^// See also: " , "\\see " )
				sub( "^//" , "" )
				printf( " * %s\n" , $0 )
			}
		}
	'
}

# HeaderFilter()
# Adds doxygen annotation to header-file comments.
#
HeaderFilter()
{
	${awk} ' BEGIN { 
		was_comment_line = 0 
		was_code_line = 0 
		re_namespace = "^[[:space:]]*namespace" 
		re_comment = "^[[:space:]]*//" 
		re_code = "^[[:space:]]*///" 
	}
	{
		is_namespace_line = match($0,re_namespace)

		is_comment_line = match($0,re_comment)
		re_comment_length = RLENGTH

		is_code_line = match($0,re_code)
		re_code_length = RLENGTH

		if( is_namespace_line )
		{
			printf( "/*! \\namespace %s */\n" , $2 )
		}

		if( is_code_line )
		{
			sub( "///" , "//" )
		}

		if( is_comment_line )
		{
			indent = substr( $0 , 1 , re_comment_length-2 )
			sub( "Class: " , "\\class " )
			sub( "Typedef: .*" , "" )
			sub( "Description: " , "" )
			sub( "See also: " , "\\see " )
			if( was_comment_line )
				sub( re_comment , indent "    " )
			else if( length(indent) )
				sub( re_comment , indent "/**<" )
			else
				sub( re_comment , indent "/** " )
		}

		if( is_code_line && !was_code_line )
		{
			print indent "\\code"
		}

		if( was_code_line && !is_code_line )
		{
			print indent "\\endcode"
		}

		if( was_comment_line && !is_comment_line )
		{
			print indent "*/"
		}

		print

		was_comment_line = is_comment_line
		was_code_line = is_code_line
	} '
}

# PostFilter()
# Deals with nested-class descriptions with a format like "class Foo // comment".
#
PostFilter()
{
	${awk} '
		{
			if( match( $0 , "^[[:space:]]*class[[:space:]][^/]*//" ) ||
			    match( $0 , "^[[:space:]]*struct[[:space:]][^/]*//" ) )
			{
				class = substr( $0 , 1 , RLENGTH-2 )
				description = substr( $0 , RLENGTH+1 )
				printf( "  /** %s */\n" , description )
				printf( "%s\n" , class )
			}
			else
			{
				print
			}
		} '
}

BasicFilter()
{
	cat
	echo '/* \\file */'
}

name="`basename \"${1}\"`"
type="`echo \"${name}\" | ${awk} -F . '{print $NF}'`"
classes="`fgrep '// Class:' \"${1}\" | wc -l`"

if test "${name}" = "gdef.h" -o "${name}" = "gnet.h"
then
	cat "${1}" | BasicFilter

elif test "${type}" = "cpp" -a "${classes}" -eq 0
then
	cat "${1}" | BasicFilter

elif test "${type}" = "cpp" -a "${classes}" -gt 0
then
	cat "${1}" | SourceFilter

else
	cat "${1}" | PreFilter | HeaderFilter | PostFilter
fi

