#
# SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
# SPDX-License-Identifier: GPL-3.0-or-later
#
# PopClient.pm
#
# A network client for driving the pop interface.
#
# Synopsis:
#
#	my $pc = new PopClient( 10101 , "localhost" ) ;
#	$pc->open() or die ;
#	$pc->login("me","secret") or die ;
#	my @list = $pc->list() ;
#	$pc->disconnect() ;
#

use strict ;
use FileHandle ;
use NetClient ;
use System ;

package PopClient ;

sub new
{
	my ( $classname , $port , $server ) = @_ ;

	$port ||= 10110 ;
	$server ||= $System::localhost ;

	my %me = (
		m_port => $port ,
		m_server => $server ,
		m_prompt => qr/\+OK[^\n]*\n/ ,
		m_timeout => 10 ,
		m_nc => undef ,
	) ;
	return bless \%me , $classname ;
}

sub port { return shift->{'m_port'} }
sub server { return shift->{'m_server'} }

sub open
{
	my ( $this , $wait ) = @_ ;
	$wait = defined($wait) ? $wait : 1 ;

	$this->{m_nc} = new NetClient( $this->{m_port} , $this->{m_server} , $this->{m_timeout} , $this->{m_prompt} ) ;
	$this->{m_nc}->read( undef , -1 ) if $wait ;
	return 1 ;
}

sub login
{
	my ( $this , $name , $pwd ) = @_ ;
	$this->{m_nc}->cmd( "user $name" ) ;
	$this->{m_nc}->cmd( "pass $pwd" ) ;
	return 1 ;
}

sub list
{
	my ( $this , $read_slowly__not_used ) = @_ ;
	my $s = $this->{m_nc}->cmd( "list" , qr/\.\r\n/ ) ;
	$s =~ s/^$$this{m_prompt}// ;
	$s =~ s/\.\r\n$// ;
	$s =~ s/\r//g ;
	return split( /\n/ , $s ) ;
}

sub disconnect
{
	my ( $this ) = @_ ;
	$this->{m_nc} = undef ;
}

1 ;
