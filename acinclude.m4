dnl 
dnl Copyright (C) 2001-2002 Graeme Walker <graeme_walker@users.sourceforge.net>
dnl 
dnl This program is free software; you can redistribute it and/or
dnl modify it under the terms of the GNU General Public License
dnl as published by the Free Software Foundation; either
dnl version 2 of the License, or (at your option) any later
dnl version.
dnl 
dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl 
dnl You should have received a copy of the GNU General Public License
dnl along with this program; if not, write to the Free Software
dnl Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
dnl 
dnl ===

AC_DEFUN([ENABLE_FHS],
[
if test "$enable_fhs" = "yes"
then
	FHS_COMPLIANCE
fi
])

AC_DEFUN([FHS_COMPLIANCE],
[
	# tweaks for fhs compliance...
	#
	prefix='/usr'
	exec_prefix='/usr'
	#
	sbindir='/usr/sbin'
	libexecdir='/usr/lib'
	localstatedir='/var'
	mandir='/usr/man'
	datadir='/usr/share'
	#
	# not used by emailrelay
	#bindir=
	#sysconfdir=
	#sharedstatedir=
	#libdir=
	#includedir=
	#oldincludedir=
	#infodir=
	#
	# emailrelay-specific
	e_sbindir="$sbindir"
	e_libexecdir="$libexecdir/$PACKAGE"
	e_docdir="$datadir/doc/$PACKAGE"
	e_initdir="/etc/init.d"
	e_spooldir="$localstatedir/spool/$PACKAGE"
	e_man1dir="$datadir/man/man1"
	e_examplesdir="$datadir/doc/$PACKAGE/examples"
])

