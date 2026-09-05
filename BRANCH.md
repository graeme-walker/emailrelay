# feature/updated-build-scripts

This branch updates the build scripts in libexec that are mostly
used on Windows, but also to build with cmake on Linux. These
changes increase the use of cmake, prepare for a GUI sub-project,
refactor so that winbuildall can produce a complete release assembly,
better support Qt6, and push more functionality into the generated
cmake files. Support for debian, RPM and Alpine Linux packaging is
also updated.

* "libresslbuild.pl" builds openssl on unix and windows using cmake,
not just on windows using nmake
* "mbedtlsbuild.pl" builds mbedtls using cmake rather than "cl" and
"link"
* "winbuild-assembly.pl" factored out of "winbuild.pl"
* "winbuildall.bat" uses "winbuild.pl --all" and "winbuild-assembly.pl"
* AutoMakeParser allows the 'vars' hash values to be code so their
value can vary through the build tree in a more obvious way
* ConfigStatus can read multiple "config.status" files in order
to support sub-packages having their own "configure" script
* the 'switches' and 'vars' of a ConfigStatus object can be
hard-coded by using ConfigStatus::parse(), in particular for
windows builds
* AutoMakeParser::read_all() takes a ConfigStatus object reference
rather than separate 'switches' and 'vars'
* BuildInfo will read a sub-package "config.status" file from
"src/gui" if present
* "make2cmake" moves more logic into the generated cmake files and
uses "list(TRANSFORM)" to allow the cmake files to be separate from
the source tree
* manifest files are no longer used
* autoconf directories rationalised (more /usr/share, less /usr/lib)
* auth and pam files are no longer installed by make install
* auth, pam and config files are installed as examples
* config file is no-clobber installed unless --disable-install-config
* debian package building updated
* new 'make apk' for Alpine Linux

