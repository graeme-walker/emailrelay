#
## SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
## SPDX-License-Identifier: GPL-3.0-or-later
#

# make (--) and make install (==)...
#
#   emailrelay-foo.sh.in
#     \--sed-> emailrelay-foo.sh
#                \==> e_examples (examples/scripts)
#   emailrelay.init.in
#     \--sed-> emailrelay
#                \==> e_initdir/ (examples/init)
#

example_scripts_static = \
	emailrelay-bcc-check.pl \
	emailrelay-check-ipaddress.pl \
	emailrelay-dkim-signer.pl \
	emailrelay-ldap-verify.py \
	emailrelay-multicast.sh \
	emailrelay-rot13.pl \
	emailrelay-sendmail.pl

example_scripts_js = \
	emailrelay-check-ipaddress.js \
	emailrelay-set-message-id.js \
	emailrelay-set-from.js \
	emailrelay-set-from.pl \
	emailrelay-edit-content.js \
	emailrelay-edit-envelope.js \
	emailrelay-resubmit.js \
	emailrelay-service-install.js

example_scripts_in = \
	emailrelay-notify.sh.in \
	emailrelay-resubmit.sh.in \
	emailrelay-submit.sh.in

example_scripts_out = \
	emailrelay-notify.sh \
	emailrelay-resubmit.sh \
	emailrelay-submit.sh

init_script_in = \
	emailrelay.init.in

init_script_out = \
	emailrelay

EXTRA_DIST = \
	$(example_scripts_static) \
	$(example_scripts_js) \
	$(example_scripts_in) \
	$(init_script_in)

e_examples_DATA = \
	$(example_scripts_js)

e_examples_SCRIPTS = \
	$(example_scripts_out) \
	$(example_scripts_static)

e_init_SCRIPTS = \
	$(init_script_out)

CLEANFILES = \
	$(example_scripts_out) \
	$(init_script_out)

do_sed = sed -e "s%__SPOOL_DIR__%${e_spooldir}%g" -e "s%__SBIN_DIR__%${sbindir}%g" -e "s%__CONF_DIR__%${e_confdir}%g" -e "s%__EGCONF_DIR__%${e_egconfdir}%g" -e "s%__RUN_DIR__%${e_rundir}%g"

emailrelay-notify.sh: emailrelay-notify.sh.in
	$(do_sed) < $(srcdir)/emailrelay-notify.sh.in > emailrelay-notify.sh
	chmod +x emailrelay-notify.sh

emailrelay-resubmit.sh: emailrelay-resubmit.sh.in
	$(do_sed) < $(srcdir)/emailrelay-resubmit.sh.in > emailrelay-resubmit.sh
	chmod +x emailrelay-resubmit.sh

emailrelay-submit.sh: emailrelay-submit.sh.in
	$(do_sed) < $(srcdir)/emailrelay-submit.sh.in > emailrelay-submit.sh
	chmod +x emailrelay-submit.sh

emailrelay: emailrelay.init.in
	$(do_sed) < $(srcdir)/emailrelay.init.in > emailrelay
	chmod +x emailrelay

