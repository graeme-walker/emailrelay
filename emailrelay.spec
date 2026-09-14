Summary: Simple e-mail message transfer agent and proxy using SMTP
Name: emailrelay
Version: 2.7rc1
Release: 1
License: GPL3
Group: System Environment/Daemons
URL: https://emailrelay.sourceforge.net
Source0: https://sourceforge.net/projects/emailrelay/files/emailrelay/%{version}/emailrelay-%{version}-src.tar.gz
BuildRequires: systemd-rpm-macros

%{!?_unitdir: %global _unitdir /usr/lib/systemd/system}

%description
E-MailRelay is a lightweight SMTP store-and-forward mail server with POP access
to spooled messages. It can be used as a personal internet mail server using
SpamAssassin spam filtering and DNSBL connection blocking, with incoming e-mail
delivered to maildir mailboxes. Store-and-forward operation is normally to a
fixed smarthost but DNS MX routing can also be configured. External scripts can
be used for address validation and for processing e-mail messages as they
are received.

E-MailRelay runs as a single process using the same non-blocking i/o model as
Squid and nginx giving excellent scalability and resource usage.

%global debug_package %{nil}
%prep
%setup -q

%build
%configure e_systemddir=%{_unitdir} --without-doxygen --with-openssl --without-mbedtls --with-pam --disable-gui --disable-testing --enable-install-pam --enable-install-config
%make_build

%install
rm -rf %{buildroot}
make install-strip DESTDIR=%{buildroot}

%post
test -x %{_datadir}/doc/emailrelay/init/emailrelay && %{_datadir}/doc/emailrelay/init/emailrelay setup || true
%systemd_post emailrelay.service

%preun
%systemd_preun emailrelay.service

%postun
%systemd_postun_with_restart emailrelay.service

%files

%ghost %{_sysconfdir}/emailrelay.auth
%config(noreplace) %{_sysconfdir}/emailrelay.conf
%config(noreplace) %attr(0644, root, root) %{_sysconfdir}/pam.d/emailrelay
%{_datadir}/emailrelay
%{_sbindir}/emailrelay
%{_sbindir}/emailrelay-passwd
%attr(2755, root, daemon) %{_sbindir}/emailrelay-submit
%{_datadir}/doc/emailrelay
%{_mandir}/man1/emailrelay-passwd.1.gz
%{_mandir}/man1/emailrelay-submit.1.gz
%{_mandir}/man1/emailrelay.1.gz
%{_unitdir}/emailrelay.service
%dir %attr(2775, root, daemon) %{_localstatedir}/spool/emailrelay

%changelog

* Sat Sep 5 2026 Graeme Walker <graeme_walker@users.sourceforge.net> - 2.6.1-1
- Updated.

