Summary: Simple e-mail message transfer agent using SMTP
Name: emailrelay
Version: 1.0.1
Release: 1
Copyright: GPL
Group: System Environment/Daemons
Source: http://emailrelay.sourceforge.net/.../emailrelay-src-1.0.1.tar.gz
BuildRoot: /tmp/emailrelay-install

%define prefix /usr

%description
E-MailRelay is a simple SMTP store-and-forward message transfer agent (MTA).
It runs as an SMTP server, storing e-mail in a local spool directory, and
then forwarding the stored messages to the next SMTP server on request. 
It can also run as a proxy server, forwarding (and optionally pre-processing)
e-mail as soon as it is received. It does not do any message routing, other
than to a local postmaster. Because of this functional simplicity it is 
extremely easy to configure, typically only requiring the address of the 
next-hop SMTP server to be put on the command line.

C++ source code is available for Linux, FreeBSD (etc) and Windows. 
Distribution is under the GNU General Public License.

%prep
%setup

%build
./configure --enable-fhs
make HAVE_DOXYGEN=no HAVE_MAN2HTML=no

%install
make install destdir=$RPM_BUILD_ROOT DESTDIR=$RPM_BUILD_ROOT HAVE_DOXYGEN=no HAVE_MAN2HTML=no

%post
test -f /usr/lib/lsb/install_initd && cd /etc/init.d && /usr/lib/lsb/install_initd emailrelay

%preun
test $1 -eq 0 && test -f /usr/lib/lsb/remove_initd && cd /etc/init.d && /usr/lib/lsb/remove_initd emailrelay

%clean
rm -rf $RPM_BUILD_ROOT

%files

/etc/init.d/emailrelay
%{prefix}/lib/emailrelay/emailrelay-poke
%{prefix}/sbin/emailrelay
%{prefix}/sbin/emailrelay-passwd
%{prefix}/sbin/emailrelay-submit
%{prefix}/share/doc/emailrelay/NEWS
%{prefix}/share/doc/emailrelay/README
%{prefix}/share/doc/emailrelay/changelog.gz
%{prefix}/share/doc/emailrelay/changelog.html
%{prefix}/share/doc/emailrelay/developer.html
%{prefix}/share/doc/emailrelay/developer.txt
%{prefix}/share/doc/emailrelay/emailrelay.css
%{prefix}/share/doc/emailrelay/examples/emailrelay-deliver.sh
%{prefix}/share/doc/emailrelay/examples/emailrelay-notify.sh
%{prefix}/share/doc/emailrelay/examples/emailrelay-process.sh
%{prefix}/share/doc/emailrelay/examples/emailrelay-resubmit.sh
%{prefix}/share/doc/emailrelay/index.html
%{prefix}/share/doc/emailrelay/readme.html
%{prefix}/share/doc/emailrelay/reference.html
%{prefix}/share/doc/emailrelay/reference.txt
%{prefix}/share/doc/emailrelay/userguide.html
%{prefix}/share/doc/emailrelay/userguide.txt
%{prefix}/share/doc/emailrelay/windows.html
%{prefix}/share/doc/emailrelay/windows.txt
%{prefix}/share/man/man1/emailrelay-passwd.1.gz
%{prefix}/share/man/man1/emailrelay-poke.1.gz
%{prefix}/share/man/man1/emailrelay-submit.1.gz
%{prefix}/share/man/man1/emailrelay.1.gz
/var/spool/emailrelay/

%changelog

* Wed Jul 3 2003 Graeme Walker <graeme_walker@users.sourceforge.net>
- Initial version.

