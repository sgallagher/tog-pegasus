
This directory defines a library to be used to compile WQL1 query
statement (which may only contain a simple SELECT statement).

The main interface is in <Pegasus/WQL/WQLParser.h> which contains an
example.

See program under tests/Parser for an example.

To generate documentation under:

    <pegasus-root>/pegasus/doc/reference/Pegasus/WQL/index.html

Type "make docxx".

WARNING: Compiling the grammar.  The Makefile includes a section to
compile the grammar that is defined in WQL.l and WQL.y.  This Makefile
allows either lex and yacc or flex/bision tools to be used for the
compilation.  

However, we have noted a significant difference in the various versions
of bison and flex.  We recommend that anybody compiling the grammar for
WQL use an up-to-date version of lex/yacc or flex/bison.  In particular,
the versions generally available in windows have proven to be out of date
and created several questionable output structures.

This has been successfully compiled on current Linux platforms using
bison 1.87 and flex verison 2.5.4.



