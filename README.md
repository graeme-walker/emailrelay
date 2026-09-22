E-MailRelay Readme
==================

Introduction
------------
E-MailRelay is a lightweight SMTP store-and-forward mail server with POP access
to spooled messages. It can be used as a personal internet mail server using
SpamAssassin spam filtering and DNSBL connection blocking, with incoming e-mail
delivered to maildir mailboxes. Store-and-forward operation is normally to a
fixed smarthost but DNS MX routing can also be configured. External scripts can
be used for address validation and for processing e-mail messages as they
are received.

![whatisit.png](doc/whatisit.png)

E-MailRelay runs as a single process using the same non-blocking i/o model as
Squid and nginx giving excellent scalability and resource usage.

Quick start
-----------
E-MailRelay can be run straight from the command-line, and on Windows you can
run `emailrelay.exe` or `emailrelay-textmode.exe` from the zip file without
going through the installation process.

To use E-MailRelay in store-and-forward mode use the `--as-server` option to
start the storage daemon in the background, and then do delivery of spooled
messages by running with `--as-client`.

![serverclient.png](doc/serverclient.png)

For example, to start a storage daemon in the background listening on port 10025
use a command like this:

        emailrelay --as-server --port 10025 --spool-dir /tmp

On Windows use `c:/temp` for testing, rather than `/tmp`.

Or to run it in the foreground:

        emailrelay --log --no-daemon --port 10025 --spool-dir /tmp

And then to forward the spooled mail to `smtp.example.com` run something
like this:

        emailrelay --as-client smtp.example.com:25 --spool-dir /tmp

To forward continuously you can add the `--poll` and `--forward-to` options to
the server command-line.

![forwardto.png](doc/forwardto.png)

For example, this starts a server that also forwards spooled-up e-mail every
minute:

        emailrelay --as-server --poll 60 --forward-to smtp.example.com:25

Or for a server that forwards each message as soon as it has been received, you
can use `--forward-on-disconnect`:

        emailrelay --as-server --forward-on-disconnect --forward-to smtp.example.com:25

To edit or filter e-mail as it passes through the server specify your filter
program with the `--filter` option, something like this:

        emailrelay --as-server --filter /tmp/set-from.js

Look for example filter scripts in the `examples` directory.

E-MailRelay can also be used as a personal internet mail server by listening on
port 25 and serving up received e-mails with POP.

![mailserver.png](doc/mailserver.png)

Use `--remote-clients` (`-r`) to allow connections from outside the local
network, define your domain name with `--domain` and use an address verifier as
a first line of defense against spammers:

        emailrelay --as-server -v -r --domain=example.com --address-verifier=account:

Enable POP access to the incoming e-mails with `--pop`, `--pop-port` and
`--pop-auth`:

        emailrelay ... --pop --pop-port 10110 --pop-auth /etc/emailrelay.auth

Set up the POP account with a user-id and password in the `--pop-auth` secrets
file. The secrets file should contain a single line of text like this:

        server plain <userid> <password>

For more information on the command-line options refer to the reference guide
or run:

        emailrelay --help --verbose

Installation
------------
For a full installation of E-MailRelay on Windows run the `emailrelay-setup`
program. On the last page choose the automatic startup option so that
E-MailRelay runs as a Windows service and use the Windows `Services` utility to
choose automatic or manual startup.

To install E-MailRelay on Linux from source or from a RPM, DEB or APK package:

		./bootstrap ; ./configure.sh ; make -j ; sudo make install
        sudo rpm -i emailrelay*.rpm
        sudo dpkg -i emailrelay*.deb
        sudo apk add --allow-untrusted emailrelay*.apk

Autostart
---------
To get the E-MailRelay server to start automatically on Linux you should check
the configuration file `/etc/emailrelay.conf` is as you want it and then run the
following commands to activate the `systemd` service:

        systemctl enable emailrelay
        systemctl start emailrelay
        systemctl status emailrelay

On other Linux systems try some combination of these commands as root to set up
and activate the E-MailRelay service:

        cp /usr/lib/emailrelay/init/emailrelay /etc/init.d/
        update-rc.d emailrelay enable
        rc-update add emailrelay
        invoke-rc.d emailrelay start
        service emailrelay start
        tail /var/log/messages
        tail /var/log/syslog

On BSD systems add this line to /etc/rc.conf:

        emailrelay_enable="YES"

Feedback
--------
To give feedback, including reviews, bug reports and feature requests, please
use the SourceForge project website at https://sourceforge.net/projects/emailrelay
