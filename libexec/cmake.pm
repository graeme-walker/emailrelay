#!/usr/bin/perl
#
# SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
# SPDX-License-Identifier: GPL-3.0-or-later
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

