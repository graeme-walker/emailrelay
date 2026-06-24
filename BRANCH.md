# feature/multiple-ports

This branch allows for multiple "--port" options on the command-line.
The set of listening sockets is the product of the set of ports and
the set of listening network addresses.

The lowest-level change is for G::OptionMap to add a numbers()
method that returns a list of unsigned integers for a multi-valued
option.

At the highest level the Main::Options structure is modified to
allow "--port" to be multi-valued and Main::Configuration uses
G::OptionMap to do an early check that the values are valid numbers.
The Main::Configuration "port" function is changed to "ports"
returning a vector.

In the network layer it is the GNet::Interfaces class that combines
the set of listening interfaces with set of port numbers. Associated
plumbing changes are needed for the GNet::MultiServer,
GNet::Listeners, GPop::Server and GSmtp::Server classes.

