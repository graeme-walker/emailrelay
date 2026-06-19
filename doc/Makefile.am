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
# doc/Makefile.am
#
# Generates nothing by default and installs only the CommonMark
# documents, png files and man pages.
#
# If GCONFIG_DOXYGEN is set then "make doxygen" creates full doxygen
# documentation under "doxygen/". Otherwise, by default, "make doxygen"
# just creates a dummy index file "doxygen/index.html".
#
# If GCONFIG_HTML is defined then the effect is to run the "htmlfiles"
# target by default and to install the generated HTML files and also
# any files found under "doxygen/".
#
# Additional pseudo-targets:
#    * htmlfiles -- builds HTML documentation using pandoc
#    * doxygen -- generates doxygen output under doxygen/
#    * sphinx -- runs sphinx-build into sphinx/
#    * website -- runs "make-sphinx --website" into website/out/
#    * release-notes -- generates RELEASE.md from the changelog
#
# Sphinx is configured in "conf.py" to generate documentation from
# CommonMark source documents, converting CommonMark to RST internally
# using MyST-Parser. The root document is configured as "contents.md".
#

md_files = \
	contents.md \
	developer.txt \
	reference.txt \
	userguide.txt \
	windows.txt

man_files = \
	emailrelay.1 \
	emailrelay-passwd.1 \
	emailrelay-submit.1

svg_files = \
	authentication.svg \
	forwardto.svg \
	serverclient.svg \
	whatisit.svg \
	mailserver.svg \
	popbyname.svg

png_files = \
	authentication.png \
	forwardto.png \
	serverclient.png \
	whatisit.png \
	download-button.png \
	mailserver.png \
	popbyname.png

html_files = \
	readme.html \
	developer.html \
	reference.html \
	userguide.html \
	windows.html \
	changelog.html

css_files = \
	emailrelay.css \
	emailrelay-doxygen.css

if GCONFIG_HTML
noinst_DATA = $(svg_files)
man1_MANS = $(man_files)
e_doc_DATA = $(md_files) $(html_files) $(css_files) $(png_files)
else
noinst_DATA = $(svg_files)
man1_MANS = $(man_files)
e_doc_DATA = $(md_files) $(png_files)
endif

EXTRA_DIST = \
	.readthedocs.yaml \
	conf.py \
	$(man_files) \
	$(md_files) \
	$(css_files) \
	$(png_files) \
	$(svg_files)

CLEANFILES = \
	*.html \
	*.1.gz \
	RELEASE.md \
	doxygen.out \
	doxygen.cfg

clean-local:
	-rm -rf doxygen/
	-rm -rf sphinx/
	-rm -rf website/

SUFFIXES = .txt .html

run_doxygen=$(top_srcdir)/libexec/run_doxygen.pl

doxygen:
	-@chmod +x $(run_doxygen)
	$(run_doxygen) "$(GCONFIG_DOXYGEN)" "$(top_srcdir)" "$(top_builddir)" doxygen doxygen.cfg

emailrelay.1.gz : emailrelay.1
	if test -n "$(GZIP)" ; then $(GZIP) -c "$(top_srcdir)/doc/emailrelay.1" > emailrelay.1.gz ; fi

emailrelay-passwd.1.gz : emailrelay-passwd.1
	if test -n "$(GZIP)" ; then $(GZIP) -c "$(top_srcdir)/doc/emailrelay-passwd.1" > emailrelay-passwd.1.gz ; fi

emailrelay-submit.1.gz : emailrelay-submit.1
	if test -n "$(GZIP)" ; then $(GZIP) -c "$(top_srcdir)/doc/emailrelay-submit.1" > emailrelay-submit.1.gz ; fi

if GCONFIG_HTML

install-data-local: install-e_docDATA
	$(mkinstalldirs) "$(DESTDIR)$(e_docdir)/doxygen"
	if test -d doxygen ; then for file in doxygen/* ; do $(INSTALL) -m 644 "$$file" "$(DESTDIR)$(e_docdir)/$$file" || true ; done ; else true ; fi

uninstall-local:
	-find "$(DESTDIR)$(e_docdir)/doxygen" -type f | xargs rm -f
	-rmdir "$(DESTDIR)$(e_docdir)/doxygen" 2>/dev/null
	-rmdir "$(DESTDIR)$(e_docdir)" 2>/dev/null

endif

$(html_files): $(md_files)
	pandoc -f markdown-raw_html-smart $(top_srcdir)/doc/reference.txt -t html -o reference.html
	pandoc -f markdown-raw_html-smart $(top_srcdir)/doc/userguide.txt -t html -o userguide.html
	pandoc -f markdown-raw_html-smart $(top_srcdir)/doc/developer.txt -t html -o developer.html
	pandoc -f markdown-raw_html-smart $(top_srcdir)/doc/windows.txt -t html -o windows.html
	pandoc -f markdown-raw_html-smart $(top_srcdir)/ChangeLog -t html -o changelog.html
	pandoc -f markdown-raw_html-smart $(top_srcdir)/README -t html -o readme.html

index.md:
	echo readme userguide reference windows developer | perl -anE 'say "- [$$_]($$_.html)" for @F' > index.md

index.html: index.md
	pandoc -f markdown -t html -s --metadata title=E-MailRelay -o index.html < index.md

.PHONY: htmlfiles
htmlfiles: $(html_files) index.html

.PHONY: sphinx-build
sphinx-build:
	sphinx-build -a -E -b html . ./sphinx-build

.PHONY: sphinx
sphinx:
	@chmod +x $(top_srcdir)/libexec/make-sphinx || true
	$(top_srcdir)/libexec/make-sphinx --examples-dir=$(top_srcdir)/bin $(top_srcdir) sphinx

.PHONY: website
website:
	@chmod +x $(top_srcdir)/libexec/make-sphinx || true
	$(top_srcdir)/libexec/make-sphinx --website --examples-dir=$(top_srcdir)/bin $(top_srcdir) website

