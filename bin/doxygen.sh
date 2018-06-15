#!/bin/sh
#
# Copyright (C) 2001-2018 Graeme Walker <graeme_walker@users.sourceforge.net>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
# ===
#
# doxygen.sh
#
# Used from doc/Makefile to run doxygen. Works correctly in a vpath build by
# creating a doxygen configuration file on the fly. Copies a dummy html
# file if doxygen is not installed.
#
# The ephemeral doxygen config file ("doxygen.cfg") is created from the
# template file ("doxygen.cfg.in"), with variable substitution for the
# root directories.
#
# usage: doxygen.sh <have-doxygen> <top-srcdir> <top-builddir> <out-sub-dir> [<doxyfile-in> [<doxyfile-out> [<missing-html>]]]
#
# eg: doxygen "$(GCONFIG_HAVE_DOXYGEN)" "$(top_srcdir)" "$(top_builddir)" doxygen.cfg.in doxygen.cfg doxygen-missing.html
#
#

HAVE_DOXYGEN="$1"
top_srcdir="$2"
top_builddir="$3"
subdir="${4-doxygen}"
doxyfile_in="${5-doxygen.cfg.in}"
doxyfile_out="${6-doxygen.cfg}"
missing_html="${7-doxygen-missing.html}"

if test "$HAVE_DOXYGEN" = "yes"
then
	cat "${top_srcdir}/doc/${doxyfile_in}" | \
		sed "s:__TOP_SRC__:${top_srcdir}:g" | \
		sed "s:__TOP_BUILD__:${top_builddir}:g" | \
		cat > "${doxyfile_out}"
	rm -f doxygen.out html/index.html 2>/dev/null
	cat "${doxyfile_out}" | doxygen - > doxygen.out 2>&1
	test -f "${subdir}/index.html"
else
	mkdir "${subdir}" 2>/dev/null
	cp -f "${top_srcdir}/doc/${missing_html}" "${subdir}/index.html"
fi

