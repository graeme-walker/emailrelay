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
# txt2html.sh
#

awk="awk"

file="${1}"
if test "${file}" = ""
then
	echo usage: `basename $0` '<txt-file>' >&2
	exit 2
fi

if test \! -f "${file}"
then
	echo `basename $0`: no such file: ${file} >&2
	exit 1
fi

title="`basename ${file}`"
if test "${2}" != ""
then
	title="${2}"
fi

# ===

Main()
{
${awk} -v title="${1}" '
BEGIN {
	printf( "<html>\n" )
	printf( "<head>\n" )
	printf( "<title>%s</title>\n" , title )
	printf( "</head>\n" )
	printf( "<body>\n" )
}

function escape( line )
{
	gsub( "&" , "\\&amp;" , line )
	gsub( "<" , "\\&lt;" , line )
	gsub( ">" , "\\&gt;" , line )
	return line
}

function dequote( line )
{
	quote = "\""
	not_quote = "[^" quote "]"
	gsub( quote not_quote "*" quote , "<b><em>&</em></b>" , line )
	gsub( "<em>" quote , "<em>" , line )
	gsub( quote "</em>" , "</em>" , line )
	return line
}

function fn( line )
{
	gsub( "[^[:space:]][^[:space:]]*\\(\\)" , "<i>&</i>" , line )
	return line
}

function output( line )
{
	printf( "%s\n" , fn(dequote(escape(line))) )
}

function tagOutput( line , tag )
{
	printf( "<%s>%s</%s>\n" , tag , fn(dequote(escape(line))) , tag )
}

function process( line , next_ )
{
	is_blank = match( line , "^[[:space:]]*$" )
	is_sub_para = match( line , "^[[:space:]][[:space:]][^[:space:]]" )
	is_code = match( line , "^[[:space:]]" ) && !is_sub_para
	is_heading = match( next_ , "^==*[[:space:]]*$" )
	is_sub_heading = match( next_ , "^--*[[:space:]]*$" )
	is_list_item = match( line , "^\\* " ) 
	is_numbered_item = match( line , "^\\([[:digit:]][[:digit:]]*\\)" )
	is_heading_line = match( line , "^==*[[:space:]]*$" )
	is_sub_heading_line = match( line , "^--*[[:space:]]*$" )

	if( is_blank )
	{
		printf( "<p><br>\n" )
	}
	else if( is_code )
	{
		tagOutput( line , "pre" )
	}
	else if( is_sub_para )
	{
		tagOutput( line , "sub" )
	}
	else if( is_list_item )
	{
		gsub( "^\\* " , "" , line )
		tagOutput( line , "li" )
	}
	else if( is_numbered_item )
	{
		gsub( "^\\([[:digit:]][[:digit:]]*\\) " , "" , line )
		tagOutput( line , "LI" )
	}
	else if( is_heading )
	{
		printf( "<h1>%s</h1>\n" , line )
	}
	else if( is_sub_heading )
	{
		printf( "<h2>%s</h2>\n" , line )
	}
	else if( !is_heading_line && !is_sub_heading_line )
	{
		output( line )
	}
}

{
	if( NR != 1 )
		process( previous , $0 )
	previous = $0
}

END {
	process( previous , "" )
	printf( "</body>\n" )
	printf( "</html>\n" )
} '
}

# ==

AugmentLists()
{
${awk} -v item_tag="${1}" -v list_tag="${2}" '
{
	line = $0
	is_list_item = match( line , "^<" item_tag ">.*</" item_tag ">$" )

	if( is_list_item && !in_list )
		printf( "<%s>\n" , list_tag )
	else if( in_list && !is_list_item )
		printf( "</%s>\n" , list_tag )

	print
	in_list = is_list_item
} '
}

# ==

Elide()
{
${awk} -v tag="${1}" '
{
	line = $0
	is_tag_line = match( line , "^<" tag ">.*</" tag ">$" )

	core = substr( line , length(tag)+3 , length(line)-length(tag)-length(tag)-5 )

	if( is_tag_line && !in_tag )
		printf( "<%s>%s" , tag , core )
	else if( is_tag_line && in_tag )
		printf( "\n%s" , core )
	else if( !is_tag_line && in_tag )
		printf( "</%s>\n%s\n" , tag , line )
	else
		print line

	in_tag = is_tag_line
} '
}

# ==

Compress()
{
${awk} '
function process( previous , line , next_ )
{
	re_blank = "^<p><br>$"
	re_heading = "^<[Hh][[:digit:]]>"
	re_pre_start = "^<pre>"
	re_pre_end = "</pre>$"
	if( match(line,re_blank) && ( match(next_,re_heading) || match(previous,re_heading) ) )
	{
	}
	else if( match(line,re_blank) && match(next_,re_pre_start) )
	{
	}
	else
	{
		print line
	}
}
{
	if( NR >= 2 )
		process( l2 , l1 , $0 )
	l2 = l1
	l1 = $0
}
END {
	process( l2 , l1 , "" )
	process( l1 , "" , "" )
}
'
}

# ==

cat "${file}" | Main "${title}" | Compress | AugmentLists li bl | AugmentLists LI ol | Elide "sub" | Elide "pre"

