noinst_SCRIPTS = emailrelay-doxygen-filter.sh emailrelay-test.sh
libexec_SCRIPTS = emailrelay.sh
pkgdata_DATA = emailrelay-notify.sh emailrelay-deliver.sh emailrelay-process.sh
EXTRA_DIST = emailrelay-doxygen-filter.sh_ emailrelay-test.sh_ emailrelay.sh_ txt2html.sh_ emailrelay-notify.sh_ emailrelay-deliver.sh_ emailrelay-process.sh_
CLEANFILES = $(noinst_SCRIPTS) $(libexec_SCRIPTS) $(pkgdata_DATA)
TESTS = emailrelay-test.sh
SUFFIXES = .sh_ .sh
.sh_.sh:
	cp $(srcdir)/../bin/$*.sh_ $*.sh 
	chmod ugo+x $*.sh
