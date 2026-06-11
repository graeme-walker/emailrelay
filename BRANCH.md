# fix/gui-translations

This improves the loading of Qt6 qtbase translations by looking for
files like "qt_<lang>.qm" or "qtbase_<lang>.qm" so that it works
for statically-linked and dynamically-linked builds.
