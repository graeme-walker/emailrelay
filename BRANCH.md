# feature/sphinx-not-doxygen

This branch modifies the documentation to use more markdown, more
sphinx and less doxygen.

Documentation text files in "doc/" are in CommonMark format, and
the markdown files and HTML files no longer included.

The "doc" makefile does not generate HTML from text files or run
doxygen unless GCONFIG_HTML and GCONFIG_DOXYGEN respectively have
been set by the configure script.

The "doc" makefile supports "make sphinx" to generate sphinx
documentation, and "make website" to generate the website (which is
the same but with a few extra tweaks). The "doc/changelog.txt"
and "doc/readme.txt" files are removed since "make sphinx" creates
them from the main README and ChangeLog files.

The "man2html" utility is no longer used, "pandoc" is needed for
"make htmlfiles", and "MyST-Parser" is required "make sphinx".
