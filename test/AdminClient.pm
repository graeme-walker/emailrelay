#
# SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
# SPDX-License-Identifier: GPL-3.0-or-later
#
# AdminClient.pm
#
# A network client to drive the admin interface.
#
# Synopsis:
#
#	my $ac = new AdminClient(10026,"localhost") ;
#	$ac->open() or die ; # first
#	$rsp = $ac->doHelp() ;
#	$ac->doTerminate() ;
#	$ac->doFlush() ;
#	$line = $ac->getline() ; # last
#

use strict ;
use FileHandle ;
use NetClient ;
use System ;

package AdminClient ;

sub new
{
	my ( $classname , $port , $server ) = @_ ;

	$port ||= 10026 ;
	$server ||= $System::localhost ;

	my %me = (
		m_port => $port ,
		m_server => $server ,
		m_prompt => qr/E-MailRelay> / ,
		m_timeout => 3 ,
		m_nc => undef ,
	) ;
	return bless \%me , $classname ;
}

sub port { return shift->{m_port} }
sub server { return shift->{m_server} }
sub doHelp { return $_[0]->{m_nc}->cmd( "help" ) }
sub doTerminate { $_[0]->{m_nc}->send( "terminate\r\n" ) }
sub doFlush { $_[0]->{m_nc}->send( "flush\r\n") }
sub doForward { $_[0]->{m_nc}->cmd( "forward") }

sub open
{
	my ( $this ) = @_ ;
	$this->{m_nc} = new NetClient( $this->{m_port} , $this->{m_server} , $this->{m_timeout} , $this->{m_prompt} ) ;
	return defined($this->{m_nc}) ;
}

sub getline
{
	my ( $this , $timeout ) = @_ ;
	my $line = $this->{m_nc}->read( qr/\n/ , $timeout ) ;
	$line = "" if !defined($line) ;
	$line =~ s/\r//g ;
	$line =~ s/\n$// ;
	return $line ;
}

1 ;
