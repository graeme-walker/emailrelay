#!/usr/bin/perl
#
# SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
# SPDX-License-Identifier: GPL-3.0-or-later
# 
# Copyright (c) 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
# ===
#
# cmake.pm
#
# Provides cmake::pick() to choose the 'best' cmake program
# on the PATH depending on its default generator. Typically
# this avoids using "perl/c/bin/cmake.exe" on Windows.
#
# Synopsis:
#    use cmake ;
#    my $cmake = cmake::pick() ;
#    system( "$cmake -B . -S ." ) ;
#

use strict ;
use IO::File ;

package cmake ;

sub pick
{
	# Returns the path of the best cmake program on the PATH,
	# or just "cmake".

	if( $^O =~ m/win/i )
	{
		my @list = () ;
		{
			my $fh = new IO::File( "where cmake |" ) ;
			while(<$fh>)
			{
				chomp( my $path = $_ ) ;
				push @list , $path ;
			}
		}
		for my $cmake ( @list )
		{
			my $fh = new IO::File( "\"$cmake\" --help |" ) ;
			while(<$fh>)
			{
				chomp( my $line = $_ ) ;
				if( $line =~ m/^\* Visual Studio/ )
				{
					return $cmake ;
				}
			}
		}
	}
	return "cmake" ;
}

1 ;

