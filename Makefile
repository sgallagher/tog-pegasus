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

rebuild: clean depend all repository config
	@ $(MAKE) -s tests

world: depend all repository config
	@ $(MAKE) -s tests

# The repository Target removes and rebuilds the CIM repository

repository:
	@ $(MAKE) -SC src/Pegasus/Compiler/load repository

config:
	@ $(MAKE) -SC src/Providers/generic/ConfigSettingProvider/load config

user:
	@ $(MAKE) -SC src/Providers/generic/UserManagerProvider/load user

registration:
	@ $(MAKE) -SC src/Providers/generic/PG_RegistrationProvider/load registration

shutdownService:
	@ $(MAKE) -SC src/Providers/generic/ShutdownProvider/load shutdownService
