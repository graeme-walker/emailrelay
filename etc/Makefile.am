#
## SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
## SPDX-License-Identifier: GPL-3.0-or-later
#

# make (--) and make install (==)...
#
#   emailrelay.auth.in
#     \--cp-> emailrelay.auth
#               \==> e_egconfdir/ (examples/config)
#   emailrelay.conf.in
#     \--sed-> emailrelay.conf
#               \==> e_egconfdir/ (examples/config)
#               \==nc=> e_confdir/ (/etc) [unless --disable-install-config]
#   emailrelay.service.in
#     \--sed-> emailrelay.service
#               \==> e_systemddir/ (examples/init)
#   emailrelay.pam
#     \==> e_egconfdir/ (examples/config)
#     \==nc=> e_pamdir/emailrelay (/etc/pam.d) [unless --disable-install-pam]
#

EXTRA_DIST = \
	emailrelay.auth.in \
	emailrelay.conf.in \
	emailrelay.cfg.in \
	emailrelay.pam \
	emailrelay.service.in \
	emailrelay-fail2ban-jail \
	emailrelay-fail2ban-filter \
	apkbuild.in \
	apkbuild.post-install

noinst_DATA = \
	emailrelay.auth

e_egconf_DATA = \
	emailrelay.pam \
	emailrelay.auth \
	emailrelay.conf

e_examples_DATA = \
	emailrelay-fail2ban-filter \
	emailrelay-fail2ban-jail

e_systemd_DATA = \
	emailrelay.service

do_sed1 = sed \
	-e "s:__RUN_DIR__:$(e_rundir):g" \
	-e "s:__SYSCONF_DIR__:$(e_confdir):g" \
	-e "s:__SBIN_DIR__:$(sbindir):g"

emailrelay.service: emailrelay.service.in
	$(do_sed1) < $(srcdir)/emailrelay.service.in > emailrelay.service

do_sed2 = sed \
	-e "s:/var/spool/emailrelay:$(e_spooldir):g" \
	-e "s:/etc:$(e_confdir):g" \
	-e "s:/run:$(e_rundir):g" \
	-e "s:/usr/local/sbin:$(sbindir):g"

emailrelay.conf: emailrelay.conf.in
	$(do_sed2) < $(srcdir)/emailrelay.conf.in > emailrelay.conf

emailrelay.auth: emailrelay.auth.in
	cp $(srcdir)/emailrelay.auth.in emailrelay.auth

CLEANFILES = \
	emailrelay \
	emailrelay.auth \
	emailrelay.conf \
	emailrelay.service

# no-clobber install of config file -- disable with 'configure --disable-install-config'
if GCONFIG_INSTALL_CONFIG
gconfig_install_config:
	-$(MKDIR_P) "$(DESTDIR)$(e_confdir)/" || true
	if test -f "$(DESTDIR)$(e_confdir)/emailrelay.conf" ; then : ; else $(INSTALL_DATA) "emailrelay.conf" "$(DESTDIR)$(e_confdir)/" ; fi
gconfig_uninstall_config:
	-rm "$(DESTDIR)$(e_confdir)/emailrelay.conf" || true
else
gconfig_install_config:
gconfig_uninstall_config:
endif

# no-clobber install of pam file -- disable with 'configure --disable-install-pam'
if GCONFIG_INSTALL_PAM
gconfig_install_pam:
	-$(MKDIR_P) "$(DESTDIR)$(e_pamdir)/" || true
	if test -f "$(DESTDIR)$(e_pamdir)/emailrelay" ; then : ; else $(INSTALL_DATA) "$(top_srcdir)/etc/emailrelay.pam" "$(DESTDIR)$(e_pamdir)/emailrelay" ; fi
gconfig_uninstall_pam:
	-rm "$(DESTDIR)$(e_pamdir)/emailrelay" || true
else
gconfig_install_pam:
gconfig_uninstall_pam:
endif

install-data-hook: gconfig_install_config gconfig_install_pam

uninstall-hook: gconfig_uninstall_config gconfig_uninstall_pam

