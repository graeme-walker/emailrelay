Summary: Simple e-mail message transfer agent using SMTP
Name: emailrelay
Version: 0.9.9
Release: 1
Copyright: GPL
Group: System Environment/Daemons
Source: http://emailrelay.sourceforge.net/.../emailrelay-src-0.9.9.tar.gz
BuildRoot: /tmp/emailrelay-install

%description
E-MailRelay is a simple SMTP store-and-forward message transfer agent (MTA).
It runs as an SMTP server, storing e-mail in a local spool directory, and
then forwarding the stored messages to the next SMTP server on request. 
It can also run as a proxy server, forwarding (and optionally pre-processing)
e-mail as soon as it is received. It does not do any message routing, other
than to a local postmaster. Because of this functional simplicity it is 
extremely easy to configure, typically only requiring the address of the 
next-hop SMTP server to be put on the command line.

C++ source code is available for Linux, FreeBSD and Windows. Distribution is
under the GNU General Public License.

%prep
%setup

%build
./configure --enable-fhs
make HAVE_DOXYGEN=no HAVE_MAN2HTML=no

%install
make install destdir=$RPM_BUILD_ROOT DESTDIR=$RPM_BUILD_ROOT HAVE_DOXYGEN=no HAVE_MAN2HTML=no

%clean
rm -rf $RPM_BUILD_ROOT

%files

/usr/lib/emailrelay/emailrelay-poke
/usr/sbin/emailrelay
/usr/sbin/emailrelay-passwd
/usr/sbin/emailrelay-submit
/usr/share/doc/emailrelay/examples/emailrelay-process.sh
/usr/share/doc/emailrelay/examples/emailrelay-notify.sh
/usr/share/doc/emailrelay/examples/emailrelay-deliver.sh
/usr/share/doc/emailrelay/examples/emailrelay-resubmit.sh
/usr/share/doc/emailrelay/developer.txt
/usr/share/doc/emailrelay/reference.txt
/usr/share/doc/emailrelay/userguide.txt
/usr/share/doc/emailrelay/windows.txt
/usr/share/doc/emailrelay/readme.html
/usr/share/doc/emailrelay/developer.html
/usr/share/doc/emailrelay/reference.html
/usr/share/doc/emailrelay/userguide.html
/usr/share/doc/emailrelay/windows.html
/usr/share/doc/emailrelay/index.html
/usr/share/doc/emailrelay/changelog.html
/usr/share/doc/emailrelay/emailrelay.css
/usr/share/doc/emailrelay/NEWS
/usr/share/doc/emailrelay/README
/usr/share/doc/emailrelay/changelog.gz
/usr/share/doc/emailrelay/doxygen
/usr/share/man/man1/emailrelay.1.gz
/usr/share/man/man1/emailrelay-passwd.1.gz
/usr/share/man/man1/emailrelay-poke.1.gz
/usr/share/man/man1/emailrelay-submit.1.gz
/etc/init.d/emailrelay

%changelog

* Mon Sep 24 2001 Graeme Walker <graeme_walker@users.sourceforge.net>
- Initial version.

