#
## SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
## SPDX-License-Identifier: GPL-3.0-or-later
#
# Makefile.am
#
# Top-level makefile
#
# Additional pseudo-targets:
#	* rpm - builds an rpm package using rpmbuild
#	* deb - builds a deb package using debhelper
#	* apk - builds an alpine apk package using abuild
#	* cmake - generates cmake files under ./build/
#	* tidy - runs cmake-tidy
#	* format - runs cmake-format
#
# Eg:
#	$ dnf install g++ make automake openssl-devel pam-devel rpm-build # rpm
#	$ ./bootstrap
#	$ ./configure.sh
#	$ make
#	$ make rpm DISRO=.rhel9
#
# or:
#	$ apt install g++ make automake libssl-dev libpam0g-dev debhelper # deb
#	$ ./bootstrap
#	$ ./configure.sh
#	$ make
#	$ make deb DISTRO=+deb13
#
# or:
#	$ apk add build-base autoconf automake git alpine-sdk abuild openssl-dev pkgconfig
#	$ ./bootstrap
#	$ ./configure.sh
#	$ make
#	$ make apk DISTRO=.alpine320 APK_UNITY=1
#
# and possibly:
#	$ make cmake ; make -C build
#	$ make tidy TIDY=clang-tidy-10
#	$ make format FORMAT=clang-format-10
#	$ make distcheck DISTCHECK_CONFIGURE_FLAGS=--disable-testing
#

EXTRA_DIST = \
	emailrelay.spec \
	configure.sh \
	ChangeLog \
	README.md \
	REUSE.toml \
	VERSION \
	bootstrap \
	autogen.sh \
	winbuild.bat \
	winbuild.pl \
	runperl.bat \
	LICENSES/FSFAP.txt \
	LICENSES/GPL-2.0-or-later.txt \
	LICENSES/GPL-3.0-or-later.txt

SUBDIRS = libexec bin src etc doc debian test m4 bsd unity

# work-round PKG_CHECK_MODULES if no pkg-config
ACLOCAL_AMFLAGS = -I m4

e_doc_DATA = AUTHORS NEWS README ChangeLog

README: README.md
	sed '/^!/{N;d}' README.md > README

RPM_ARCH ?= x86_64
APK_ARCH ?= x86_64
DEB_ARCH ?= amd64
DISTRO ?=

TAR=emailrelay-$(VERSION).tar.gz
ZIP=emailrelay-$(VERSION)-src.zip
RPM=emailrelay-$(VERSION)-1$(DISTRO).$(RPM_ARCH).rpm
DEB=emailrelay_$(VERSION)-1$(DISTRO)_$(DEB_ARCH).deb
APK=emailrelay-$(VERSION)-r0$(DISTRO).$(APK_ARCH).apk

.PHONY: strip
strip:
	$(MAKE) -C src/main strip

.PHONY: rpm
rpm: $(RPM)

$(RPM): dist
	-rm -f "emailrelay-$(VERSION)-1.$(RPM_ARCH).rpm"
	-mkdir -p "rpmbuild/BUILD" "rpmbuild/SOURCES" "rpmbuild/SPECS" 2>/dev/null
	cp $(TAR) "rpmbuild/SOURCES/emailrelay-$(VERSION)-src.tar.gz"
	cp $(top_srcdir)/emailrelay.spec "rpmbuild/SPECS/"
	-chown root:root "rpmbuild/SPECS/emailrelay.spec"
	rpmbuild --define "_topdir `pwd`/rpmbuild" -ba "`pwd`/rpmbuild/SPECS/emailrelay.spec"
	mv "rpmbuild/RPMS/$(RPM_ARCH)/emailrelay-$(VERSION)-1.$(RPM_ARCH).rpm" "$(RPM)"
	-ls -l emailrelay*.rpm

.PHONY: deb
deb: $(DEB)

$(DEB): dist
	-mv emailrelay-$(VERSION) emailrelay-$(VERSION).old.$$$$ || true
	tar -xzf $(TAR)
	cd emailrelay-$(VERSION) && dpkg-buildpackage -us -uc -b -jauto
	mv "emailrelay_$(VERSION)_$(DEB_ARCH).deb" "$(DEB)"
	-ls -l emailrelay*.deb

.PHONY: deb-src
deb-src: dist
	-mv emailrelay-$(VERSION) emailrelay-$(VERSION).old.$$$$ || true
	tar -xzf $(TAR)
	dpkg-source --build emailrelay-$(VERSION)

.PHONY: tidy
tidy:
	$(MAKE) -C unity tidy

.PHONY: unity
unity:
	$(MAKE) -C unity unity

.PHONY: small
small:
	$(MAKE) -C unity small

.PHONY: format
format:
	$(MAKE) -C src format

.PHONY: cmake
cmake:
	@chmod +x libexec/make2cmake || true
	libexec/make2cmake
	test -d build || mkdir build
	cd build && cmake -DCMAKE_MAKE_PROGRAM:FILEPATH=/usr/bin/make -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
	@echo now run make from the '"build"' directory

.PHONY: qmake
qmake:
	@chmod +x libexec/make2qmake || true
	libexec/make2qmake

.PHONY: tar
tar: $(TAR)

$(TAR):
	$(MAKE) dist

.PHONY: zip
zip: $(ZIP)

$(ZIP): $(TAR)
	-mkdir makezip.tmp 2>/dev/null
	-@rm -rf makezip.tmp/* 2>/dev/null
	tar -C makezip.tmp --strip-components=1 -xzf $(TAR)
	sh -c "cd makezip.tmp && zip -r ../$(ZIP) ."

APKBUILD: $(TAR)
	sed -e '/^sha512sums=/q' -e '/^sha512sums=/d' -e 's/__VERSION__/'"`cat $(top_srcdir)/VERSION`/g" -e 's/__PKGVER__/'"`cat $(top_srcdir)/VERSION | sed 's/rc.*//'`/g" < "$(top_srcdir)/etc/apkbuild.in" > APKBUILD
	if test "0$(APK_UNITY)" -eq 1 ; then sed -i -e 's;^_unity=.*;_unity=1;' APKBUILD ; fi
	echo "sha512sums=\"" >> APKBUILD
	sha512sum < "$(TAR)" | sed 's/-/'$(TAR)'/' >> APKBUILD
	echo "\"" >> APKBUILD
	grep -A 999 "^sha512sums=" < "$(top_srcdir)/etc/apkbuild.in" | grep -v '^sha512sums=' >> APKBUILD || true

APKBUILD.post-install:
	cp "$(top_srcdir)/etc/apkbuild.post-install" APKBUILD.post-install

.PHONY: apk
apk: apk-key apk-d apk-build apk-package apk-rename

.PHONY: apk-d
apk-d: $(TAR) APKBUILD APKBUILD.post-install
	mkdir -p apk.d
	cp "$(TAR)" apk.d/
	cp APKBUILD.post-install apk.d/emailrelay.post-install
	cp APKBUILD apk.d/
	sed -i -e 's;^source=.*;source="$(TAR)";' apk.d/APKBUILD
	cd apk.d && unset REPODEST && abuild -P "`pwd`" -F clean unpack
	cp APKBUILD apk.d/

.PHONY: apk-build
apk-build:
	cd apk.d && unset REPODEST && abuild -P "`pwd`" -F prepare mkusers build

.PHONY: apk-key
apk-key:
	if grep -q "^PACKAGER_PRIVKEY=" "${HOME}/.abuild/abuild.conf" 2>/dev/null ; then : ; else abuild-keygen -n -a ; fi

.PHONY: apk-package
apk-package:
	cd apk.d && unset REPODEST && abuild -P "`pwd`" -F rootpkg

.PHONY: apk-rename
apk-rename:
	mv apk.d/`basename \`pwd\``/$(APK_ARCH)/emailrelay-$(VERSION)-r0.apk emailrelay-$(VERSION)-r0$(DISTRO).$(APK_ARCH).apk
	mv apk.d/`basename \`pwd\``/$(APK_ARCH)/emailrelay-doc-$(VERSION)-r0.apk emailrelay-doc-$(VERSION)-r0$(DISTRO).$(APK_ARCH).apk
	mv apk.d/`basename \`pwd\``/$(APK_ARCH)/emailrelay-openrc-$(VERSION)-r0.apk emailrelay-openrc-$(VERSION)-r0$(DISTRO).$(APK_ARCH).apk
	-ls -l emailrelay*.apk

.PHONY: mbedtls
mbedtls:
	test -d mbedtls || git clone https://salsa.debian.org/debian/mbedtls.git
	make -C mbedtls/library WINDOWS=0
	-@echo now run ./configure.sh

dist-hook:
	find "$(distdir)" -type f \( -name \*.bat -o -name \*.rc -o -name \*.manifest -o -name \*.mc -o -name resource.h \) | while read -r f ; do sed 's/\r$$//; s/$$/\r/' "$$f" > "$$f.tmp" && mv "$$f.tmp" "$$f" ; done

.PHONY: release-notes
release-notes:
	$(MAKE) -C doc release-notes

.PHONY: git-hooks
git-hooks:
	-cp $(top_srcdir)/libexec/git-pre-commit .git/hooks/pre-commit

