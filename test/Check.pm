#
# SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
# SPDX-License-Identifier: GPL-3.0-or-later
#
# Check.pm
#
# Various assertion functions that all reduce down
# to Check::that(ok,text).
#

use strict ;
use Carp ;
use FileHandle ;

package Check ;

sub that
{
	my ( $ok , @args ) = @_ ;
	if( !$ok )
	{
		Carp::croak( "[ " . join(": ",map {my $s=$_=~s/\n/\\n/gr=~s/\r/\\r/gr;$s} grep{defined($_)} @args) . " ]" ) ;
	}
}

sub ok
{
	that( @_ ) ;
}

sub fileEmpty
{
	my ( $path , $more ) = @_ ;
	if( -f $path )
	{
		my ($dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size,$atime,$mtime,$ctime,$blksize,$blocks) = stat($path);
		Check::that( $size == 0 , "file not empty" , $path , $more ) ;
	}
}

sub fileExists
{
	my ( $path , $more ) = @_ ;
	Check::that( -f $path , "file does not exist" , $path , $more ) ;
}

sub fileNotEmpty
{
	my ( $path , $more ) = @_ ;
	my ($dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size,$atime,$mtime,$ctime,$blksize,$blocks) = stat($path);
	Check::that( -f $path && $size > 0 , "file missing or empty" , $path , $more ) ;
}

sub running
{
	my ( $pid , $more ) = @_ ;
	my $n = defined($pid) ? System::processIsRunning($pid) : 0 ;
	Check::that( $n == 1 , "process not running" , $pid , $more ) ;
}

sub notRunning
{
	my ( $pid , $more ) = @_ ;
	my $n = defined($pid) ? System::processIsRunning($pid) : 0 ;
	Check::that( $n == 0 , "process still running" , $pid , $more ) ;
}

sub numeric
{
	my ( $s , $more ) = @_ ;
	Check::that( defined($s) && $s =~ m/[[:digit:]]+/ , "invalid numeric string" , "[$s]" , $more ) ;
}

sub fileDeleted
{
	my ( $path , $more ) = @_ ;
	Check::that( ! -f $path , "file still exists" , $path , $more ) ;
}

sub fileMatchCount
{
	my ( $expr , $count , $more ) = @_ ;
	my @files = System::glob_( $expr ) ;
	my $n = scalar(@files) ;
	Check::that( $n == $count , "unexpected number of matching files (got $n, expected $count) [$expr]" , $more ) ;
}

sub fileOwner
{
	my ( $path , $name , $more ) = @_ ;
	my ($dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size,$atime,$mtime,$ctime,$blksize,$blocks) = stat($path);
	my $expected = System::uid($name) ;
	Check::that( $uid == $expected , "unexpected file owner" , $path , "$uid!=$expected($name)" , $more ) ;
}

sub fileGroup
{
	my ( $path , $name , $more ) = @_ ;
	my ($dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size,$atime,$mtime,$ctime,$blksize,$blocks) = stat($path);
	my $expected = System::gid($name) ;
	Check::that( $gid == $expected , "unexpected file group" , $path , "$gid!=$expected($name)" , $more ) ;
}

sub fileMode
{
	my ( $path , $mode_ , $more ) = @_ ;
	my ($dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size,$atime,$mtime,$ctime,$blksize,$blocks) = stat($path);
	$mode &= 0777 ;
	Check::that( $mode == $mode_ , "unexpected file permissions" , $path , $mode."!=".$mode_ , $more ) ;
}

sub processRealUser
{
	my ( $pid , $name ) = @_ ;
	my $actual = System::realUser($pid) ;
	my $expected = System::uid($name) ;
	Check::that( $actual == $expected , "wrong real user: [$actual]!=[$expected]" ) ;
}

sub processEffectiveUser
{
	my ( $pid , $name ) = @_ ;
	my $actual = System::effectiveUser($pid) ;
	my $expected = System::uid($name) ;
	Check::that( $actual == $expected , "wrong effective user: [$actual]!=[$expected]" ) ;
}

sub processSavedUser
{
	my ( $pid , $name ) = @_ ;
	my $actual = System::savedUser($pid) ;
	my $expected = System::uid($name) ;
	Check::that( $actual == $expected , "wrong saved user: [$actual]!=[$expected]" ) ;
}

sub processRealGroup
{
	my ( $pid , $name ) = @_ ;
	my $actual = System::realGroup($pid) ;
	my $expected = System::gid($name) ;
	Check::that( $actual == $expected , "wrong real group: [$actual]!=[$expected]" ) ;
}

sub processEffectiveGroup
{
	my ( $pid , $name ) = @_ ;
	my $actual = System::effectiveGroup($pid) ;
	my $expected = System::gid($name) ;
	Check::that( $actual == $expected , "wrong effective group: [$actual]!=[$expected]" ) ;
}

sub _fileLineCount
{
	my ( $path , $string ) = @_ ;
	my $fh = new FileHandle( $path ) ;
	my $n = 0 ;
	while(<$fh>)
	{
		my $line = $_ ;
		chomp $line ;
		if( !defined($string) || $line =~ m/$string/ ) { $n++ }
	}
	return $n ;
}

sub fileLineCount
{
	my ( $path , $count , $string , $more ) = @_ ;
	my $n = _fileLineCount( $path , $string ) ;
	Check::that( $n == $count , "invalid matching line count" , $path , $n."!=".$count , $more ) ;
}

sub fileLineCountLessThan
{
	my ( $path , $count , $string , $more ) = @_ ;
	my $n = _fileLineCount( $path , $string ) ;
	Check::that( $n < $count , "invalid matching line count" , $path , $n."!<".$count , $more ) ;
}

sub allFilesContain
{
	my ( $glob , $re , $more ) = @_ ;
	my @files = System::glob_( $glob ) ;
	for my $file ( @files )
	{
		fileContains( $file , $re , $more ) ;
	}
}

sub noFileContains
{
	my ( $glob , $re_or_list , $more ) = @_ ;
	my @files = System::glob_( $glob ) ;
	for my $file ( @files )
	{
		fileDoesNotContain( $file , $re_or_list , $more ) ;
	}
}

sub fileContains
{
	my ( $path , $re , $more , $count ) = @_ ;
	my $fh = new FileHandle( $path ) ;
	my $n = 0 ;
	while(<$fh>)
	{
		my $line = $_ ;
		chomp $line ;
		if( !defined($re) || $line =~ m/$re/ ) { $n++ }
	}
	my $ok = defined($count) ? ($n == $count) : ($n > 0) ;
	Check::that( $ok , "file does not contain expected string" , $path , "[$re]" , $more ) ;
}

sub fileContainsEither
{
	my ( $path , $re1 , $re2 , $more ) = @_ ;
	my $fh = new FileHandle( $path ) ;
	my $n = 0 ;
	while(<$fh>)
	{
		my $line = $_ ;
		chomp $line ;
		if( $line =~ m/$re1/ ) { $n++ }
		if( $line =~ m/$re2/ ) { $n++ }
	}
	Check::that( $n > 0 , "file does not contain one of strings" , $path , "[$re1] [$re2]" , $more ) ;
}

sub fileDoesNotContain
{
	my ( $path , $re_or_list , $more ) = @_ ;
	die if !defined($re_or_list) ;
	my @re_list = ref($re_or_list) eq "ARRAY" ? @$re_or_list : ($re_or_list) ;
	my $fh = new FileHandle( $path ) ;
	my $n = 0 ;
	while(<$fh>)
	{
		my $line = $_ ;
		chomp $line ;
		for my $re ( @re_list )
		{
			if( $line =~ m/$re/ ) { $n++ }
		}
	}
	Check::that( $n <= 0 , "file contains unexpected string" , $path , "[@re_list]" , $more ) ;
}

sub match
{
	my ( $s , $re , $more ) = @_ ;
	my $ok = ( $s =~ $re ) ;
	Check::that( $ok , "string does not match regexp [$re]: got [$s]" , $more ) ;
}

1 ;
