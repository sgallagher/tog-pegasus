# Pegasus top level make file
# options are
# Make rebuild
# Make world
# Make tests - Executes the complete test suite
# Make repository - Rebuilds the Pegasus repository
#
ROOT = .

include $(ROOT)/mak/config.mak

# Defines subdirectorys to go to recursively

# DIRS = src cgi
DIRS = src

# This is a recurse make file

include $(ROOT)/mak/recurse.mak

# rebuild target cleans, setup dependencies, compiles all and builds 
# repository

rebuild: clean depend all repository
	@ $(MAKE) -s tests

world: depend all repository
	@ $(MAKE) -s tests

# The repository Target removes and rebuilds the CIM repository

repository:
	@ $(MAKE) -SC src/Pegasus/Compiler/load repository
