EXTRA_DIST = developer.txt reference.txt userguide.txt windows.txt index.html emailrelay.1 emailrelay-poke.1 doxygen_header.html graphics/bullet.gif

noinst_SCRIPTS = .dox
pkgdata_DATA = readme.html developer.html reference.html userguide.html man.html index.html windows.html changelog.html
CLEANFILES = $(noinst_SCRIPTS) html *.ht readme.html developer.html reference.html userguide.html man.html

SUFFIXES = .txt .html .ht

filter=$(top_builddir)/bin/emailrelay-doxygen-filter.sh
filter_src=$(top_srcdir)/bin/emailrelay-doxygen-filter.sh_
converter=$(top_builddir)/bin/txt2html.sh
converter_src=$(top_srcdir)/bin/txt2html.sh_

.txt.html:
	$(converter) -a "$(AWK)" $< > $*.html

.txt.ht:
	$(converter) -a "$(AWK)" -x $< > $*.ht

$(filter): $(filter_src)
	cp $(filter_src) $(filter)
	chmod ugo+x $(filter)

$(converter): $(converter_src)
	cp $(converter_src) $(converter)
	chmod ugo+x $(converter)

.dox: $(filter)
	if test "$(HAVE_DOXYGEN)" = "yes" ; then cat $(top_srcdir)/src/main/doxygen.cfg | sed "s:__TOP_SRC__:$(top_srcdir):g" | sed "s:__TOP_BUILD__:$(top_builddir):g" | doxygen - && touch .dox ; else echo no doxygen ; fi

man.html: emailrelay.1
	if test "$(HAVE_MAN2HTML)" = "yes" ; then man2html emailrelay.1 > man.html ; else echo no man2html ; fi

developer.html reference.html userguide.html: $(converter)

readme.html: $(top_srcdir)/README $(converter)
	$(converter) -a "$(AWK)" $(top_srcdir)/README > readme.html

changelog.html: $(top_srcdir)/ChangeLog $(converter)
	$(converter) -a "$(AWK)" $(top_srcdir)/ChangeLog > changelog.html

install-data-local:
	$(mkinstalldirs) $(destdir)$(mandir)/man1
	$(INSTALL) $(top_srcdir)/doc/emailrelay.1 $(destdir)$(mandir)/man1/emailrelay.1
	$(INSTALL) $(top_srcdir)/doc/emailrelay-poke.1 $(destdir)$(mandir)/man1/emailrelay-poke.1
	$(mkinstalldirs) $(destdir)$(pkgdatadir)/graphics
	$(INSTALL) $(top_srcdir)/doc/graphics/bullet.gif $(destdir)$(pkgdatadir)/graphics/bullet.gif
	$(mkinstalldirs) $(destdir)$(pkgdatadir)/html
	if test "$(HAVE_DOXYGEN)" = "yes" ; then for file in html/* ; do $(INSTALL) $$file $(destdir)$(pkgdatadir)/$$file ; done ; fi

