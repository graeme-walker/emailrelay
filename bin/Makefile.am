noinst_SCRIPTS = emailrelay-filter.sh emailrelay-test.sh
libexec_SCRIPTS = emailrelay.sh
pkgdata_DATA = emailrelay-notify.sh emailrelay-deliver.sh
EXTRA_DIST = txt2html.sh $(noinst_SCRIPTS) $(libexec_SCRIPTS) $(pkgdata_DATA)
CLEANFILES = $(noinst_SCRIPTS) $(libexec_SCRIPTS) $(pkgdata_DATA)
TESTS = emailrelay-test.sh
SUFFIXES = .sh_ .sh
.sh_.sh:
	cp $(srcdir)/../bin/$*.sh_ $*.sh 
	chmod ugo+x $*.sh
