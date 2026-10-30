#!/bin/sh
#
# SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
# SPDX-License-Identifier: GPL-3.0-or-later
#
# make-manifest.sh
#
# Creates a windows manifest file, typically for a mingw build where
# the linker does not support /MANIFEST etc.
#
# usage: make-manifest.sh [<level> [<name> [<description> [<version>]]]]
#          level := {asInvoker|highestAvailable|requireAdministrator}
#
# See also:
#  * https://msdn.microsoft.com/en-us/library/bb756929.aspx
#  * https://msdn.microsoft.com/en-us/library/aa374191.aspx
#

level="$1"
name="$2"
description="$3"
version="$4"
if test "$level" = "" ; then level="asInvoker" ; fi
if test "$version" = "" ; then version="1.0.0.0" ; fi

Manifest()
{
	_level="$1"
	_name="$2"
	_description="$3"
	_version="$4"

cat <<EOF
<?xml version="1.0" encoding="UTF-8" standalone="yes" ?>
<assembly xmlns="urn:schemas-microsoft-com:asm.v1" manifestVersion="1.0">
EOF
if test "$_name" != ""
then
cat <<EOF
 <assemblyIdentity version="$_version" name="$_name" type="win32" />
EOF
fi
if test "$_description" != ""
then
cat <<EOF
 <description>$_description</description>
EOF
fi
if test "$_level" != ""
then
cat <<EOF
 <trustInfo xmlns="urn:schemas-microsoft-com:asm.v2">
  <security>
   <requestedPrivileges>
    <requestedExecutionLevel
     level="$_level"
     uiAccess="false" />
   </requestedPrivileges>
  </security>
 </trustInfo>
EOF
fi
cat <<EOF
</assembly>
EOF
}

Manifest "$level" "$name" "$description" "$version" | sed 's/$/\r/'

