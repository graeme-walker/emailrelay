Summary: Simple e-mail message transfer agent using SMTP
Name: emailrelay
Version: 0.9.8
Release: 1
Copyright: GPL
Group: System Environment/Daemons
Source: http://emailrelay.sourceforge.net/.../emailrelay-src-0.9.8.tar.gz
BuildRoot: /tmp/emailrelay-install

%description
E-MailRelay is a simple SMTP store-and-forward message transfer agent (MTA).
It runs as an SMTP server, storing e-mail in a local spool directory, and
then forwarding the stored messages to a downstream SMTP server on request. 
It can also run as a proxy server, forwarding (and optionally pre-processing)
e-mail as soon as it is received. It does not do any message routing, other
than to a local postmaster. Because of this functional simplicity it is 
extremely easy to configure, typically only requiring the address of the 
downstream SMTP server to be put on the command line.

C++ source code is available for Linux, FreeBSD and Windows. Distribution is
under the GNU General Public License.

%prep
%setup

%build
./configure
make HAVE_DOXYGEN=no

%install
make install destdir=$RPM_BUILD_ROOT DESTDIR=$RPM_BUILD_ROOT HAVE_DOXYGEN=no

%clean
rm -rf $RPM_BUILD_ROOT

%files

/usr/local/libexec/emailrelay-passwd
/usr/local/libexec/emailrelay-poke
/usr/local/libexec/emailrelay-submit
/usr/local/libexec/emailrelay.sh
/usr/local/sbin/emailrelay
/usr/local/var/spool/emailrelay/empty_file
/usr/local/share/emailrelay/emailrelay-notify.sh
/usr/local/share/emailrelay/emailrelay-deliver.sh
/usr/local/share/emailrelay/emailrelay-process.sh
/usr/local/share/emailrelay/readme.html
/usr/local/share/emailrelay/developer.html
/usr/local/share/emailrelay/reference.html
/usr/local/share/emailrelay/userguide.html
/usr/local/share/emailrelay/man.html
/usr/local/share/emailrelay/index.html
/usr/local/share/emailrelay/windows.html
/usr/local/share/emailrelay/changelog.html
/usr/local/share/emailrelay/graphics/bullet.gif
/usr/local/man/man1/emailrelay.1
/usr/local/man/man1/emailrelay-passwd.1
/usr/local/man/man1/emailrelay-poke.1

%changelog

* Mon Sep 24 2001 Graeme Walker <graeme_walker@users.sourceforge.net>
- Initial version.

