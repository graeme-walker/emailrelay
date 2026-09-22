#
# SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
# SPDX-License-Identifier: FSFAP
#
# emailrelay-check-ipaddress.pl
#
# An example "--filter" script that verifies the submitting client's IP
# address. The IP address is read from the envelope file. Invalid IP
# addresses are rejected by deleting the two message files and exiting
# with the special exit code of 100. Note that this checks the IP
# address very late in the submission process; a firewall or DNSBL
# check might work better.
#

use strict ;
use warnings ;
use FileHandle ;
$SIG{__DIE__} = sub { (my $e = join(" ",@_)) =~ s/\n/ /g ; print "<<error>>\n<<error: $e>>\n" ; exit 99 } ;

my %allow = (
	"127.0.0.1" => 1 ,
	"1.1.1.1" => 1 ,
	# etc
) ;

my $content = $ARGV[0] or die "usage error\n" ;
my $envelope = $ARGV[1] or die "usage error\n" ;
my $fh = new FileHandle( $envelope ) or die "cannot open envelope file: $!\n" ;
my $txt ;
{
	local $/ = undef ;
	$txt = <$fh> ;
}
my ( $ip ) = ( $txt =~ m/X-MailRelay-Client: (\S*)/m ) ;
if( $allow{$ip} )
{
	exit( 0 ) ;
}
else
{
	print "<<not allowed>>\n<<not allowed: $ip>>\n" ;
	unlink( $content ) ;
	unlink( $envelope ) ;
	exit( 100 ) ;
}

