Readme.txt -- doc/providerspec directory

This directory is the source of documentation for the Pegasus Provider interface document.
The documentation is created from files in this directory using the doc++ adn gawk
utility systems.  The make file in this directory services as the core file for
documentation creation.

Note that the documentation is created from the files in this directory and the Pegasus
header files that represent the interfaces.  The classes and methods in those files must
be documented using the doc++ comments and tags.

The resulting HTML documentation is saved in the directory specificed for output in
the makefile, normally a subdirectory to PEGASUS_HOME.

The documentation is defined using the tags provided by Doc++ and HTML.

NOTE: The file PegasusSpec.dxx is the top-level file for this documentation.  It defines
the outline of the document and the list of other documentation and header files to be used
to create the document.

Karl Schopmeyer, September 4 2001