#
## SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
## SPDX-License-Identifier: GPL-3.0-or-later
#

# make (--) and make install [--enable-bsd] (==)...
#
#   emailrelay-bsd.sh.in
#     \--sed-> emailrelay
#                \==> e_bsdinitdir/ (/usr/local/etc/rc.d)
#

if GCONFIG_BSD
EXTRA_DIST = emailrelay-bsd.sh.in
e_bsdinit_SCRIPTS = emailrelay
CLEANFILES = emailrelay
else
EXTRA_DIST = emailrelay-bsd.sh.in
e_bsdinit_SCRIPTS =
CLEANFILES =
endif

do_sed = sed -e "s%__SBIN_DIR__%${sbindir}%g" -e "s%__INIT_DIR__%${e_initdir}%g"

emailrelay: emailrelay-bsd.sh.in
	$(do_sed) < $(srcdir)/emailrelay-bsd.sh.in > emailrelay
	chmod +x emailrelay

