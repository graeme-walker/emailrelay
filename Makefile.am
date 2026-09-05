#
## Copyright (C) 2001-2024 Graeme Walker <graeme_walker@users.sourceforge.net>
## 
## This program is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
## 
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
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
#	$ sudo make rpm
#
# or:
#	$ apt install g++ make automake libssl-dev libpam0g-dev debhelper # deb
#	$ ./bootstrap
#	$ ./configure.sh
#	$ make
#	$ sudo make deb
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
	LICENSE \
	VERSION \
	README.md \
	bootstrap \
	autogen.sh \
	winbuild.bat \
	winbuild.pl \
	runperl.bat

SUBDIRS = libexec bin src etc doc debian test m4 bsd po unity

# work-round PKG_CHECK_MODULES if no pkg-config
ACLOCAL_AMFLAGS = -I m4

e_doc_DATA = AUTHORS NEWS README ChangeLog

RPM_ROOT ?= ${HOME}/rpmbuild
#ARCH ?= $(shell arch)
ARCH ?= x86_64
RPM=$(RPM_ROOT)/RPMS/$(ARCH)/emailrelay-$(VERSION)-1.$(ARCH).rpm
TAR=emailrelay-$(VERSION).tar.gz
ZIP=emailrelay-$(VERSION)-src.zip
DEB=emailrelay_$(VERSION)_$(ARCH).deb

.PHONY: strip
strip:
	$(MAKE) -C src/main strip

.PHONY: rpm
rpm: $(RPM)

$(RPM): dist
	-rm -rf $(RPM_ROOT)/BUILD/emailrelay-$(VERSION)-1.$(ARCH)
	test ! -d $(RPM_ROOT)/BUILD/emailrelay-$(VERSION)-1.$(ARCH)
	-mkdir -p $(RPM_ROOT)/BUILD $(RPM_ROOT)/SOURCES $(RPM_ROOT)/SPECS 2>/dev/null
	cp $(TAR) $(RPM_ROOT)/SOURCES/emailrelay-$(VERSION)-src.tar.gz
	cp $(top_srcdir)/emailrelay.spec $(RPM_ROOT)/SPECS/
	-chown root:root $(RPM_ROOT)/SPECS/emailrelay.spec
	-rm -f $(RPM)
	test ! -f $(RPM)
	rpmbuild -ba $(RPM_ROOT)/SPECS/emailrelay.spec
	test -f $(RPM)

.PHONY: deb
deb: $(DEB)

$(DEB): dist
	-mv emailrelay-$(VERSION) emailrelay-$(VERSION).old.$$$$ || true
	tar -xzf $(TAR)
	cd emailrelay-$(VERSION) && dpkg-buildpackage -us -uc -b -jauto

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
	echo "sha512sums=\"" >> APKBUILD
	sha512sum < "$(TAR)" | sed 's/-/'$(TAR)'/' >> APKBUILD
	echo "\"" >> APKBUILD
	grep -A 999 "^sha512sums=" < "$(top_srcdir)/etc/apkbuild.in" | grep -v '^sha512sums=' >> APKBUILD || true

APKBUILD.post-install:
	cp "$(top_srcdir)/etc/apkbuild.post-install" APKBUILD.post-install

.PHONY: apkbuild
apkbuild: APKBUILD APKBUILD.post-install

.PHONY: apk
apk: $(TAR) APKBUILD APKBUILD.post-install
	if grep -q "^PACKAGER_PRIVKEY=" "${HOME}/.abuild/abuild.conf" 2>/dev/null ; then : ; else abuild-keygen -i -n -a ; fi
	mkdir -p "apk.d/${USER}/build"
	cp "$(TAR)" "apk.d/${USER}/build/$(TAR)"
	cp APKBUILD.post-install "apk.d/${USER}/build/emailrelay.post-install"
	cp APKBUILD "apk.d/${USER}/build/APKBUILD"
	sed -i -e 's;^source=.*;source="$(TAR)";' "apk.d/${USER}/build/APKBUILD"
	cd "apk.d/${USER}/build" && abuild clean unpack && cd ../..
	cp APKBUILD "apk.d/${USER}/build/APKBUILD"
	cd "apk.d/${USER}/build" && abuild prepare mkusers build rootpkg && cd ../..
	cp "${HOME}/packages/${USER}/x86_64/"emailrelay*.apk .
	@echo "+++ " doas apk add emailrelay-`cat $(top_srcdir)/VERSION | sed 's/rc.*//'`-r0.apk

.PHONY: mbedtls
mbedtls:
	test -d mbedtls || git clone https://salsa.debian.org/debian/mbedtls.git
	make -C mbedtls/library WINDOWS=0
	-@echo now run ./configure.sh

dist-hook:
	find "$(distdir)" -type f \( -name \*.bat -o -name \*.rc -o -name \*.manifest -o -name \*.mc -o -name resource.h \) | while read -r f ; do sed 's/\r$$//; s/$$/\r/' "$$f" > "$$f.tmp" && mv "$$f.tmp" "$$f" ; done

