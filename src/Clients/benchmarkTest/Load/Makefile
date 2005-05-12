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
include $(PEGASUS_ROOT)/mak/config.mak
include $(PEGASUS_ROOT)/mak/configschema.mak
include $(PEGASUS_ROOT)/mak/test.mak
include $(PEGASUS_ROOT)/mak/commands.mak

ifdef TESTID
   BenchmarkMOF_NAME = benchmarkProvider_$(TESTID)
else
   BenchmarkMOF_NAME = benchmarkProvider
endif

PROVIDERNS=root/benchmark
PROVIDERNSDIRECTORY=$(REPOSITORY_ROOT)/root\#benchmark

CIMMOFL = cimmofl

repository:
	@ $(ECHO) +++++ Removing existing repository named $(PROVIDERNS)
	@$(RMREPOSITORY) $(PROVIDERNSDIRECTORY)
	@ $(ECHO) +++++ Loading CIM_Core$(CIM_SCHEMA_VER) into $(PROVIDERNS) namespace ...
	@ $(CIMMOFL) "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-I$(CIM_SCHEMA_DIR)" \
              "-n$(PROVIDERNS)" $(ALLOW_EXPERIMENTAL) $(CIM_SCHEMA_DIR)/CIM_Core$(CIM_SCHEMA_VER).mof
	@ $(ECHO) +++++ Loading $(BenchmarkMOF_NAME) class definitions into $(PROVIDERNS) namespace ...
	@ $(CIMMOFL) "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-n$(PROVIDERNS)" $(BenchmarkMOF_NAME).mof
	@ $(ECHO) +++++ Registering benchmark Provider   ...
	@ $(CIMMOFL) "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-n$(INTEROPNS)" $(BenchmarkMOF_NAME)R.mof

repositoryServer:
	@ $(ECHO) +++++ Loading CIM_Core$(CIM_SCHEMA_VER) into $(PROVIDERNS) namespace ...
	@ cimmof "-I$(CIM_SCHEMA_DIR)" "-n$(PROVIDERNS)" $(ALLOW_EXPERIMENTAL) $(CIM_SCHEMA_DIR)/CIM_Core$(CIM_SCHEMA_VER).mof
	@ $(ECHO) +++++ Loading $(BenchmarkMOF_NAME) class definitions into $(PROVIDERNS) namespace ...
	@ cimmof "-n$(PROVIDERNS)" $(BenchmarkMOF_NAME).mof
	@ $(ECHO) +++++ Registering benchmark Provider   ...
	@ cimmof "-n$(INTEROPNS)" $(BenchmarkMOF_NAME)R.mof

removerepository:
	@ $(ECHO) +++++ Removing existing repository named $(PROVIDERNS)
	@ $(RMREPOSITORY) $(PROVIDERNSDIRECTORY)

registerproviders:
	@ $(ECHO) +++++ Registering benchmark Provider   ...
	@ cimmof "-n$(INTEROPNS)" benchmarkProviderR.mof

unregister:
	$(MAKE) -i unregisterproviders

unregisterproviders:
	cimprovider -r -m benchmarkProviderModule

changeconfig:
	$(MAKE) -i cimstart
	$(MAKE) unregister
	$(MAKE) -i cimstop
	$(MAKE) removerepository
	$(MAKE) cimstart
ifdef TESTID
	$(MAKE) repositoryServer TESTID=$(TESTID)
else
	$(MAKE) repositoryServer
endif


depend:

sub:

misc:

tests:

poststarttests:

general:

clean:

