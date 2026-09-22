#
# SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
# SPDX-License-Identifier: GPL-3.0-or-later
#
# Verifier.pm
#
# A wrapper for running the "emailrelay_test_verifier" program.
#
# See also: Helper
#

use strict ;
use Helper ;

package Verifier ;

sub new
{
	my ( $classname , $port ) = @_ ;
	return bless { h => new Helper( "emailrelay_test_verifier" , $port ) } , $classname ;
}

sub port { return shift->{h}->port(@_) }
sub logfile { return shift->{h}->logfile(@_) }
sub exe { return shift->{h}->exe(@_) }
sub run { return shift->{h}->run(@_) }
sub pid { return shift->{h}->pid(@_) }
sub kill { return shift->{h}->kill(@_) }
sub cleanup { return shift->{h}->cleanup(@_) }

1 ;
