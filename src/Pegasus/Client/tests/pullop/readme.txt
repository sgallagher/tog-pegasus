Pegasus test client to test pull operations.  Executes a single pull
sequence determined by the Type parameter consisting of an open
request and a number of pull requests.

The program options determine
   - The server (default is to the local host)
   - Parameters of the pull operation including class name,
     interoperation timeout, max object count, etc.
   - If the results are to be compared with the corresponding non pull
     operation (ex. e (the openEnumerateInstances, pull, ...) would
     be compared with enumerateInstances on the same class.  This automatically
     tests for things like properties returned, use of classorigin, etc.
   - Level or verbosity of status and error information

The goal of this program is to provide a relatively complete test for
the operation of the DMTF pull operations (client and server) that can
be executed from the command line.

The Makefile defines a broad set of tests (both correct and return) that
use pullop as the execution tool.

To see the options for pullop pullop -h or pullop --help.


There is another client program that executes a test of a number of
Pull errors (See PullErrors).






