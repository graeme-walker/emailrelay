EXTRA_DIST = developer.txt reference.txt userguide.txt index.html emailrelay.1 emailrelay-poke.1 index.html
noinst_SCRIPTS = .dox
pkgdata_DATA = readme.html developer.html reference.html userguide.html man.html index.html
CLEANFILES = $(pkgdata_DATA) $(noinst_SCRIPTS) html

SUFFIXES = .txt .html

filter=$(srcdir)/../bin/emailrelay-filter.sh
converter=$(srcdir)/../bin/txt2html.sh

.txt.html:
	$(converter) $< > $*.html

.dox: $(filter)
	if test "$(HAVE_DOXYGEN)" = "yes" ; then cat $(top_srcdir)/src/main/doxygen.cfg | sed "s:__TOP__:$(top_srcdir):g" | doxygen - && touch .dox ; else echo no doxygen ; fi

man.html: emailrelay.1
	if test "$(HAVE_MAN2HTML)" = "yes" ; then man2html emailrelay.1 > man.html ; else echo no man2html ; fi

$(filter): $(filter)_
	@cp $(filter)_ $(filter)
	@chmod ugo+x $(filter)

$(converter): $(converter)_
	@cp $(converter)_ $(converter)
	@chmod ugo+x $(converter)

developer.html reference.html userguide.html: $(converter)

readme.html: $(top_srcdir)/README $(converter)
	$(converter) $(top_srcdir)/README > readme.html

install-data-local:
	$(mkinstalldirs) $(destdir)$(mandir)/man1
	$(INSTALL) $(top_srcdir)/doc/emailrelay.1 $(destdir)$(mandir)/man1/emailrelay.1
	$(INSTALL) $(top_srcdir)/doc/emailrelay-poke.1 $(destdir)$(mandir)/man1/emailrelay-poke.1
	$(mkinstalldirs) $(destdir)$(pkgdatadir)/html
	if test "$(HAVE_DOXYGEN)" = "yes" ; then for file in html/* ; do $(INSTALL) $$file $(destdir)$(pkgdatadir)/$$file ; done ; fi

