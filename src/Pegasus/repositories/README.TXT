This subdirectory is the container directory for implementation of the repository.

The Makefile in this directory determines which repository implementation is
compiled and installed in the runtime.

As opposed to other container directories, only ONE repository implementation
can be compiled and installed since they all use the same object name.

It is our intention to extend this to be more flexible so that the repository
implementation can be selected by configuration but for the moment we have not
identified a real use model for this so that change is delayed.

Karl Schopmeyer
14 November 2001