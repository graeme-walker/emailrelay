clean up option parser

This makes G::GetOpt the principal interface for option parsing, fully
subsuming G::Options, G::OptionMap and G::OptionParser. This allows
a richer implementation of G::OptionReader to support dropin files
etc. and it also resolves an overlap between the GUI
ServerConfiguration class and Main::CommandLine.

The support for dropin files and for UAPI-style configuration files is
included but not currently used: G::GetOpt allows for a command-line
option to specify a mode for G::OptionReader but it is never set, so
G::OptionReader::Config::parse() is called with an empty string and
defaults to "single" mode.

