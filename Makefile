# Pegasus top level make file
# options are
# Make rebuild
# Make world
# Make tests - Executes the complete test suite
# Make repository - Rebuilds the Pegasus repository
#
ROOT = .

include $(ROOT)/mak/config.mak

# This is a recurse make file
# Defines subdirectorys to go to recursively

# DIRS = src cgi
DIRS = src test

# Define the inclusion of the recurse.mak file to execute the next
# level of makefiles defined by the DIRS variable

include $(ROOT)/mak/recurse.mak

# rebuild target cleans, setup dependencies, compiles all and builds 
# repository

rebuild: clean depend all repository
	@ $(MAKE) -s tests

world: depend all repository
	@ $(MAKE) -s tests

# The repository Target removes and rebuilds the CIM repository

repository:
	@ $(MAKE) -SC Schemas/Pegasus repository

testrepository:
	@ $(MAKE) -SC src/Providers/sample/Load repository
	@ $(MAKE) -SC test/wetest repository

removetestrepository:
	@ $(MAKE) -SC src/Providers/sample/Load removerepository
	@ $(MAKE) -SC test/wetest removerepository

# the collections of tests that we run with the server active.
# For now, these are centralized and do not include startup
# and shutdown of the server.

activetests:
	$(MAKE) --directory=pegasus/test -f Makefile clean
	TestClient
	Client
	$(MAKE) --directory=test -f Makefile tests
