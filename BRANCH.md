# feature/small-build-update

This withdraws the build-size optimisation that uses conditional
compilation of unused functions, as controlled by the G_LIB_SMALL
preprocessor symbol. Similar size improvements can now be realised
through compiler options. See configure.sh.
