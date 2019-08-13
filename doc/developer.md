E-MailRelay Developer Guide
===========================

Principles
----------
The main principles in the design of E-MailRelay can be summarised as:

* Minimal third-party dependencies
* Windows/Unix portability without #ifdefs
* Event-driven, non-blocking, single-threaded networking code
* Functionality without imposing policy

Dependencies
------------
E-MailRelay started life at a time when Linux had no decent package manager and
Windows was in the grip of DLL hell. As a result, a key principle is that it
has no dependencies other than a decent C++ runtime. Since that time OpenSSL
has been introduced as a dependency to support [TLS][] encryption, and the optional
configuration and installation GUI has been developed using the Qt toolkit.

In those early years multi-threading support in C++ libraries was poor, so up
until version 2.0 the code was single-threaded throughout.

Portability
-----------
The E-MailRelay code is mostly written in C++-1998, but using some features of
C++-2011. A C++-1998 compiler can be used, but multi-threading will be disabled.

The header files `gdef.h` in `src/glib` is used to fix up some compiler
portability issues such as missing standard types, non-standard system headers
etc. Conditional compilation directives (`#ifdef` etc.) are largely confined
this file in order to improve readability.

Windows/Unix portability is generally addressed by providing a common class
declaration with two implementations. The implementations are put into separate
source files with a `_unix` or `_win32` suffix, and if necessary a 'pimple' (or
'Bridge') pattern is used to keep the o/s-specific details out of the header.
If only small parts of the implementation are o/s-specific then there can be
three source files per header. For example, `gsocket.cpp`, `gsocket_win32.cpp`
and `gsocket_unix.cpp` in the `src/gnet` directory.

Underscores in source file names are used exclusively to indicate build-time
alternatives.

Event model
-----------
The E-MailRelay server uses non-blocking socket i/o, with a select() event loop.
This event model means that the server can handle multiple network connections
simultaneously from a single thread, and even if multi-threading is disabled at
build-time the only blocking occurs when external programs are executed (see
`--filter` and `--address-verifier`).

This event model can make the code more complicated than the equivalent
multi-threaded approach since (for example) it is not possible to wait for a
complete line of input to be received from a remote [SMTP][] client because there
might be other connections that need servicing half way through.

The advantages of a non-blocking event model are discussed in the well-known
[C10K Problem](http://www.kegel.com/c10k.html) document.

At higher levels the C++ slot/signal design pattern is used to propagate events
between objects (not to be confused with operating system signals). The
slot/signal implementation has been simplified compared to Qt or boost by not
supporting signal multicasting, so each signal connects to no more than one
slot.

Module structure
----------------
The main C++ libraries in the E-MailRelay code base are as follows:

*   `glib`

    Low-level classes for file-system abstraction, date and time representation,
    string utility functions, logging, command line parsing etc.

*   `gssl`

    A thin layer over the third-party TLS libraries.

*   `gnet`

    Network and event-loop classes.

*   `gauth`

    Implements various authentication mechanisms.

*   `gsmtp`

    SMTP protocol and message-store classes.

*   `gpop`

    POP3 protocol classes.

All of these libraries are portable between Unix-like systems and Windows.

Under Windows there is an additional library `win32` for the user interface.

SMTP class structure
--------------------
The message-store functionality uses three abstract interfaces: `MessageStore`,
`NewMessage` and `StoredMessage`. The `NewMessage` interface is used to create
messages within the store, and the `StoredMessage` interface is used for
reading and extracting messages from the store. The concrete implementation
classes based on these interfaces are respectively `FileStore`, `NewFile` and
`StoredFile`.

Protocol classes such as `GSmtp::ServerProtocol` receive network and timer
events from their container and use an abstract `Sender` interface to send
network data. This means that the protocols can be independent of the network
and event loop framework.

The interaction between the SMTP server protocol class and the message store is
mediated by the `ProtocolMessage` interface. Two main implementations of this
interface are available: one for normal spooling (`ProtocolMessageStore`), and
another for immediate forwarding (`ProtocolMessageForward`). The `Decorator`
pattern is used whereby the forwarding class uses an instance of the storage
class to do the message storing and filtering, while adding in an instance
of the `GSmtp::Client` class to do the forwarding.

Message filtering (`--filter`) is implemented via an abstract `Filter`
interface. Concrete implementations are provided for doing nothing, running an
external executable program and talking to an external network server.

The protocol, processor and message-store interfaces are brought together by the
high-level `GSmtp::Server` and `GSmtp::Client` classes. Dependency injection is
used to create the concrete instances of the `ProtocolMessage` and `Filter`
interfaces.

Event handling and exceptions
-----------------------------
The use of non-blocking i/o in the network library means that most processing
operates within the context of an i/o event or timeout callback, so the top
level of the call stack is nearly always the event loop code. This can make
catching C++ exceptions a bit awkward compared to a multi-threaded approach
because it is not possible to put a single catch block around a particular
high-level feature.

The event loop delivers asynchronous socket events to the `EventHandler`
interface, timer events to the `TimerBase` interface, and 'future' events to the
`FutureEventCallback` interface. If any of the these event handlers throws an
exception then the event loop will catch it and deliver it back to an exception
handler through the `onException()` method of an associated `ExceptionHandler`
interface. If an exception is thrown out of _this_ callback then the event loop
code lets it propagate back to `main()`, typically terminating the program.

Every pointer to an event callback interface is associated with an
`ExceptionHandler`. The default `ExceptionHandler` is the `EventLoop`
singleton, and a call to its `onException()` method terminates the event loop.

This leads to a programming model where key objects are instantiated on the
heap and these objects delete themselves when they receive certain events from
the event loop. In the `GNet` library it is the `ServerPeer` and `HeapClient`
classes that do this lifetime management; instances of these classes delete
themselves when the associated network connection goes away and they
implement the `ExceptionHandler` interface so that they schedule their own
deletion when an exception is thrown.

Special smart pointers are sometimes used for these self-deleting classes; the
smart pointer does not delete the contained object when it is reset, it just
tells the object to delete itself with a zero-length timer and then releases it
for garbage collection.

Multi-threading
---------------
Multi-threading can be used as a build-time option to make DNS lookup and the
execution of helper programs asynchronous; if std::thread is available then it
is used in a future/promise pattern to wrap up `getaddrinfo()` and `waitpid()`
system calls. The shared state comprises only the parameters and return results
from these system calls, and synchronisation back to the main thread uses the
event loop (see `GNet::FutureEvent`).

E-MailRelay GUI
---------------
The optional GUI program `emailrelay-gui` uses the Qt toolkit for its user
interface components. The GUI can run as an installer or as a configuration
helper, depending on whether it can find an installation `payload`. Refer to
the comments in `src/gui/guimain.cpp` for more details.

The user interface runs as a stack of dialog-box pages with forward and back
buttons at the bottom. Once the stack has been completed by the user then each
page is asked to dump out its state as a set of key-value pairs (see
`src/gui/pages.cpp`). These key-value pairs are processed by an installer class
into a list of action objects (in the `Command` design pattern) and then the
action objects are run in turn. In order to display the progress of the
installation each action object is run within a timer callback so that the Qt
framework gets a chance to update the display between each one.

During development the user interface pages and the installer can be tested
separately since the interface between them is a simple text stream containing
key-value pairs.

When run in configure mode the GUI normally ends up simply editing the
`emailrelay.conf` file (or `emailrelay-start.bat` on Windows) and/or the
`emailrelay.auth` secrets file.

When run in install mode the GUI expects to unpack all the E-MailRelay files
from the payload into target directories. (The payload used to be a single
archive file appended to the executable, but it is now simple directory
tree that lives alongside the GUI exectuable or inside the Mac application
bundle.)

Windows packaging
-----------------
On Windows E-MailRelay is packaged as a zip file containing the executables
(including the emailrelay GUI as `emailrelay-setup.exe`), documentation, and a
`payload` directory tree. The payload contains many of the same files all over
again, and while this duplication is not ideal it is at least straightforward.

The Qt tool `windeployqt` is used to add run-time dependencies, such as the
Qt DLLs.

Unix packaging
--------------
On Unix-like operating systems it is more natural to use some sort of package
derived from the `make install` process rather than an installer program, so
the emailrelay GUI is not normally used.

Top-level makefile targets `dist`, `deb` and `rpm` can be used to create a
binary tarball, a debian package, and an RPM package respectively.

Source control
--------------
The source code is stored in the SourceForge `svn` repository. A working
copy can be checked out as follows:

        $ svn co https://svn.code.sf.net/p/emailrelay/code/trunk emailrelay

Compile-time features
---------------------
Compile-time features can be selected with options passed to the `configure`
script. These include the following:

* Debug-level logging (`--enable-debug`)
* Configuration GUI (`--enable-gui`)
* [PAM][] support (`--with-pam`)

Use `./configure --help` to see a complete list of options and refer to
`acinclude.m4` for more detailed comments.




[PAM]: https://en.wikipedia.org/wiki/Linux_PAM
[SMTP]: https://en.wikipedia.org/wiki/Simple_Mail_Transfer_Protocol
[TLS]: https://en.wikipedia.org/wiki/Transport_Layer_Security

_____________________________________
Copyright (C) 2001-2018 Graeme Walker