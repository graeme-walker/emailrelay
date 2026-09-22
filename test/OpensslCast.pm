#
# SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
# SPDX-License-Identifier: GPL-3.0-or-later
#
# OpensslCast.pm
#
# Creates a set of actors:
#
#      Alice    Bob    Malory("Alice")
#        ^       ^       ^
#        |       |       |
#      Carol    Dave     |
#        ^       ^       |
#         \     /        |
#          Trent       Trudy
#           ^  |        ^  |
#           +--+        +--+
#
# Synopsis:
#	use OpensslCast ;
#	OpensslCast::create( "certificates" ) ;
#
# Optionally preceded by:
#	use Openssl ;
#	$Openssl::openssl = Openssl::search() ;
#
# And then:
#	use OpensslFileStore ;
#	$fs = new OpensslFileStore( "certificates" ) ;
#	$keyfile = $fs->infile( "alice.key" ) ;
#	$pemfile = $fs->catfile( "alice.key" , "alice.crt" ) ;
#
# Each actor has a ".key" file for their private key and a ".crt"
# file for their certificate.
#
# When creating a concatenated certificate the component
# certificates must be ordered from leaf to root.
#

use strict ;
use Openssl ;
use OpensslFileStore ;

package OpensslCast ;

sub create
{
	# Creates the standard cast of actors:
	# Alice -> Carol -> Trent, Bob -> Dave -> Trent,
	# and Malory -> Trudy.

	my ( $dir ) = @_ ;
	my $state = _push() ;
	eval
	{
		my $fs = new OpensslFileStore( $dir ) ;
		my $openssl = new Openssl( $fs ) ;
		$openssl->selfcert( "trent" , "Trent" ) ;
		$openssl->selfcert( "trudy" , "Trudy" ) ;
		$openssl->genkey( "alice" ) ;
		$openssl->genkey( "bob" ) ;
		$openssl->genkey( "carol" ) ;
		$openssl->genkey( "dave" ) ;
		$openssl->genkey( "malory" ) ;
		$openssl->sign( "carol" , "Carol" , "trent" , 1 ) ;
		$openssl->sign( "alice" , "Alice" , "carol" ) ;
		$openssl->sign( "dave" , "Dave" , "trent" , 1 ) ;
		$openssl->sign( "bob" , "Bob" , "dave" ) ;
		$openssl->sign( "malory" , "Alice" , "trudy" ) ;
		$fs->cleanup( 'tmp' , 'cat' , 'log' ) ;
	} ;
	chomp( my $e = $@ ) ;
	if( $e )
	{
		_pop( $state ) ;
		die $e ;
	}
}

sub _push
{
	my %state = (
		log_cat_fn => $OpensslFileStore::log_cat_fn ,
		log_del_fn => $OpensslFileStore::log_del_fn ,
		outpath_fn => $OpensslFileStore::outpath_fn ,
		inpath_fn => $OpensslFileStore::inpath_fn ,
	) ;
	$OpensslFileStore::log_cat_fn = sub {} ;
	$OpensslFileStore::log_del_fn = sub {} ;
	$OpensslFileStore::outpath_fn = sub { my ($dir,$fname)=@_ ; return "$dir/$fname" }  ;
	$OpensslFileStore::inpath_fn = sub { my ($dir,$fname)=@_ ; return "$dir/$fname" } ;
	return \%state ;
}

sub _pop
{
	my ( $state ) = @_ ;
	$OpensslFileStore::log_cat_fn = $state->{log_cat_fn} ;
	$OpensslFileStore::log_del_fn = $state->{log_del_fn} ;
	$OpensslFileStore::outpath_fn = $state->{outpath_fn} ;
	$OpensslFileStore::inpath_fn = $state->{inpath_fn} ;
}

1 ;
