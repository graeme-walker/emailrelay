//
// SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
// SPDX-License-Identifier: FSFAP
//
// emailrelay-check-ipaddress.js
//
// An example "--filter" script for Windows that verifies the submitting
// client's IP address. The IP address is read from the envelope file.
// Invalid IP addresses are rejected by deleting the two message files and
// exiting with the special exit code of 100. Note that this checks the
// IP address very late in the submission process; a firewall or DNSBL check
// might work better.
//
try
{
	var content = WScript.Arguments(0) ;
	var envelope = WScript.Arguments(1) ;
	var fs = WScript.CreateObject( "Scripting.FileSystemObject" ) ;
	var ts = fs.OpenTextFile( envelope , 1 , false ) ;
	var txt = ts.ReadAll() ;
	ts.Close() ;
	var re = new RegExp( "X-MailRelay-Client: (\\S*)" , "m" ) ;
	var ip = txt.match(re)[1] ;
	var ok = ip === "1.1.1.1" ; /// edit here
	if( ok )
	{
		WScript.Quit( 0 ) ;
	}
	else
	{
		WScript.StdOut.WriteLine( "<<not allowed>>" ) ;
		fs.DeleteFile( envelope ) ;
		fs.DeleteFile( content ) ;
		WScript.Quit( 100 ) ;
	}
}
catch( e )
{
	// report errors using the special <<...>> markers
	WScript.StdOut.WriteLine( "<<error>>" ) ;
	WScript.StdOut.WriteLine( "<<" + e + ">>" ) ;
	WScript.Quit( 1 ) ;
}

