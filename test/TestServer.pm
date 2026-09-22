#
# SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
# SPDX-License-Identifier: GPL-3.0-or-later
#
# TestServer.pm
#
# A wrapper for running the "emailrelay_test_server" program.
#
# See also: emailrelay_test_server.cpp
#
# Synopsis:
#
#	use TestServer ;
#	$TestServer::bin_dir = "." ;
#	my $ts = new TestServer( 10025 ) ;
#	$ts->run() ;
#	$ts->kill() ;
#	$ts->cleanup() ;
#	kill 15 , @TestServer::pid_list ;
#

use strict ;
use FileHandle ;
use System ;

package TestServer ;

our @pid_list = () ;
our $bin_dir = "." ;

sub new
{
	my ( $classname , $port ) = @_ ;

	$port = defined($port) ? $port : 10025 ;

	my %me = (
		m_port => $port ,
		m_exe => System::exe( $bin_dir , "emailrelay_test_server" ) ,
		m_logfile => System::tempfile("test-server.log") ,
		m_outfile => System::tempfile("test-server.out") ,
		m_pidfile => System::tempfile("test-server.pid") ,
		m_pid => undef ,
	) ;
	my $this = bless \%me , $classname ;
	$this->_check() ;
	return $this ;
}

sub exe { return shift->{m_exe} }
sub pid { return shift->{m_pid} }
sub port { return shift->{m_port} }

sub _check
{
	my ( $this ) = @_ ;
	if( ! -x $this->exe() )
	{
		die "invalid test server executable [".$this->exe()."]" ;
	}
	System::unlink( $this->{m_pidfile} ) if -f $this->{m_pidfile} ;
	if( -f $this->{m_pidfile} )
	{
		die "cannot remove old pidfile [".$this->{m_pidfile}."]" ;
	}
}

sub run
{
	my ( $this , $sw , $wait_cs ) = @_ ;
	$sw = "" if !defined($sw) ;
	$wait_cs ||= 50 ;

	my $cmd = System::sanepath($this->exe())." --log-file $$this{m_logfile} --loopback --port $$this{m_port} --pid-file $$this{m_pidfile} $sw" ;
	my $full_cmd = System::commandline( $cmd , { stdout => $this->{m_outfile} , stderr => $this->{m_outfile} , background => 1 } ) ;
	System::log_( "running [$full_cmd]" ) ;
	system( $full_cmd ) ;

	my $pid = System::waitForPid( $this->{m_pidfile} ) ;
	$this->{m_pid} = $pid ;
	push @pid_list , $pid if $pid ;
	return $pid && System::processIsRunning($pid) ;
}

sub log
{
	my ( $this ) = @_ ;
	return $this->{m_logfile} ;
}

sub kill
{
	my ( $this ) = @_ ;
	Check::numeric( $this->pid() ) ;
	System::kill_( $this->pid() ) ;
}

sub cleanup
{
	my ( $this ) = @_ ;
	if( $this->pid() ) { $this->kill() }
	System::unlink( $this->{m_pidfile} ) ;
	System::unlink( $this->{m_logfile} ) ;
}

1 ;

