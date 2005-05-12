#//%2005////////////////////////////////////////////////////////////////////////
#//
#// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
#// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
#// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation, The Open Group.
#// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
#// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
#// EMC Corporation; VERITAS Software Corporation; The Open Group.
#//
#// Permission is hereby granted, free of charge, to any person obtaining a copy
#// of this software and associated documentation files (the "Software"), to
#// deal in the Software without restriction, including without limitation the
#// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
#// sell copies of the Software, and to permit persons to whom the Software is
#// furnished to do so, subject to the following conditions:
#// 
#// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
#// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
#// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
#// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
#// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
#// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
#// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
#// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#//
#//==============================================================================
# Pegasus top level make file
# options are
# Make rebuild
# Make world
# Make tests - Executes the complete test suite
# Make repository - Rebuilds the Pegasus repository
#
ROOT = .

include $(ROOT)/env_var.status
include $(ROOT)/mak/config.mak

# This is a recurse make file
# Defines subdirectorys to go to recursively

# DIRS = src cgi
DIRS = src test rpm Schemas

# Define the inclusion of the recurse.mak file to execute the next
# level of makefiles defined by the DIRS variable

include $(ROOT)/mak/recurse.mak

# rebuild target cleans, setup dependencies, compiles all and builds 
# repository

FORCE:

rebuild: clean depend all repository
	@ $(MAKE) -s tests

world: depend all repository
	@ $(MAKE) -s tests

# The repository Target removes and rebuilds the CIM repository

# Note: Arguments must be quoted to preserve upper case characters in VMS.
repository: FORCE
	@ $(MAKE) "-SC" Schemas/Pegasus repository

repositoryclean: FORCE
	@ $(RMREPOSITORY) $(REPOSITORY_ROOT)

repositoryServer: FORCE
	@ $(MAKE) "-SC" Schemas/Pegasus repositoryServer

testrepository: FORCE
	@ $(MAKE) "-SC" src/Providers/sample/Load repository
	@ $(MAKE) "-SC" test/wetest repository
	@ $(MAKE) "-SC" src/Clients/benchmarkTest/Load repository
	@ $(MAKE) "-SC" src/Pegasus/CQL/CQLCLI repository
	@ $(MAKE) "-SC" src/Pegasus/Query/QueryExpression/tests repository
	@ $(MAKE) "-SC" src/Providers/TestProviders/Load repository
	@ $(MAKE) "-SC" src/Pegasus/ControlProviders/QueryCapabilitiesProvider/tests repository

testrepositoryServer: FORCE
	@ $(MAKE) "-SC" src/Providers/sample/Load repositoryServer
	@ $(MAKE) "-SC" test/wetest repositoryServer
	@ $(MAKE) "-SC" src/Clients/benchmarkTest/Load repositoryServer
	@ $(MAKE) "-SC" src/Pegasus/CQL/CQLCLI repositoryServer
	@ $(MAKE) "-SC" src/Pegasus/Query/QueryExpression/tests repositoryServer
	@ $(MAKE) "-SC" src/Providers/TestProviders/Load repositoryServer
	@ $(MAKE) "-SC" src/Pegasus/ControlProviders/QueryCapabilitiesProvider/tests repositoryServer

removetestrepository: FORCE
	@ $(MAKE) "-SC" src/Providers/sample/Load removerepository
	@ $(MAKE) "-SC" test/wetest removerepository
	@ $(MAKE) "-SC" src/Clients/benchmarkTest/Load removerepository
	@ $(MAKE) "-SC" src/Providers/TestProviders/Load removerepository

config:
	@ $(ROOT)/SetConfig_EnvVar

all: messages 

messages: rootbundle

rootbundle: 
	$(MAKE) --directory=$(PEGASUS_ROOT)/src/utils/cnv2rootbundle -f Makefile

# the collections of tests that we run with the server active.
# For now, these are centralized and do not include startup
# and shutdown of the server.

activetests: FORCE
	$(MAKE) --directory=$(PEGASUS_ROOT)/test -f Makefile clean
	$(PEGASUS_ROOT)/bin/TestClient
	$(PEGASUS_ROOT)/bin/Client
	$(MAKE) --directory=$(PEGASUS_ROOT)/test -f Makefile tests
