EXTRA_DIST = developer.txt reference.txt userguide.txt windows.txt index.html emailrelay.1 emailrelay-passwd.1 emailrelay-poke.1 emailrelay-submit.1 doxygen_header.html graphics/bullet.gif emailrelay.css

noinst_SCRIPTS = .dox
pkgdata_DATA = readme.html developer.html reference.html userguide.html man.html index.html windows.html changelog.html
CLEANFILES = $(noinst_SCRIPTS) html *.ht readme.html developer.html reference.html userguide.html man.html

SUFFIXES = .txt .html .ht

stylesheet=emailrelay.css
filter=$(top_builddir)/bin/emailrelay-doxygen-filter.sh
filter_src=$(top_srcdir)/bin/emailrelay-doxygen-filter.sh_
converter=$(top_builddir)/bin/txt2html.sh
converter_src=$(top_srcdir)/bin/txt2html.sh_
converter_helper1=$(top_builddir)/bin/txt2mu.sh
converter_helper1_src=$(top_srcdir)/bin/txt2mu.sh_
converter_helper2=$(top_builddir)/bin/mu2html.sh
converter_helper2_src=$(top_srcdir)/bin/mu2html.sh_
converter_helper3=$(top_builddir)/bin/expand.sh
converter_helper3_src=$(top_srcdir)/bin/expand.sh_

.txt.html:
	$(converter) -a "$(AWK)" $< $(stylesheet) > $*.html

.txt.ht:
	$(converter) -a "$(AWK)" -x $< $(stylesheet) > $*.ht

$(filter): $(filter_src)
	cp $(filter_src) $(filter)
	chmod ugo+x $(filter)

$(converter): $(converter_src) $(converter_helper1) $(converter_helper2) $(converter_helper3)
	cp $(converter_src) $(converter)
	chmod ugo+x $(converter)

$(converter_helper1): $(converter_helper1_src)
	cp $(converter_helper1_src) $(converter_helper1)
	chmod ugo+x $(converter_helper1)

$(converter_helper2): $(converter_helper2_src)
	cp $(converter_helper2_src) $(converter_helper2)
	chmod ugo+x $(converter_helper2)

$(converter_helper3): $(converter_helper3_src)
	cp $(converter_helper3_src) $(converter_helper3)
	chmod ugo+x $(converter_helper3)

.dox: $(filter)
	if test "$(HAVE_DOXYGEN)" = "yes" ; then cat $(top_srcdir)/src/main/doxygen.cfg | sed "s:__TOP_SRC__:$(top_srcdir):g" | sed "s:__TOP_BUILD__:$(top_builddir):g" | doxygen - && touch .dox ; else echo no doxygen ; fi

man.html: emailrelay.1
	if test "$(HAVE_MAN2HTML)" = "yes" ; then man2html emailrelay.1 > man.html ; else echo no man2html ; fi

developer.html reference.html userguide.html: $(converter)

readme.html: $(top_srcdir)/README $(converter)
	$(converter) -a "$(AWK)" $(top_srcdir)/README $(stylesheet) > readme.html

changelog.html: $(top_srcdir)/ChangeLog $(converter)
	$(converter) -a "$(AWK)" $(top_srcdir)/ChangeLog $(stylesheet) > changelog.html

install-data-local:
	$(mkinstalldirs) $(destdir)$(mandir)/man1
	$(INSTALL) $(top_srcdir)/doc/emailrelay.1 $(destdir)$(mandir)/man1/emailrelay.1
	$(INSTALL) $(top_srcdir)/doc/emailrelay-passwd.1 $(destdir)$(mandir)/man1/emailrelay-passwd.1
	$(INSTALL) $(top_srcdir)/doc/emailrelay-poke.1 $(destdir)$(mandir)/man1/emailrelay-poke.1
	$(mkinstalldirs) $(destdir)$(pkgdatadir)/graphics
	$(INSTALL) $(top_srcdir)/doc/graphics/bullet.gif $(destdir)$(pkgdatadir)/graphics/bullet.gif
	$(mkinstalldirs) $(destdir)$(pkgdatadir)/html
	if test "$(HAVE_DOXYGEN)" = "yes" ; then for file in html/* ; do $(INSTALL) $$file $(destdir)$(pkgdatadir)/$$file ; done ; fi

uninstall-local:
	-rm -f $(destdir)$(pkgdatadir)/graphics/bullet.gif
	-rm -f $(destdir)$(pkgdatadir)/html/* 2>/dev/null
	-rm -f $(destdir)$(mandir)/man1/emailrelay.1 
	-rm -f $(destdir)$(mandir)/man1/emailrelay-passwd.1 
	-rm -f $(destdir)$(mandir)/man1/emailrelay-poke.1 

