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
# Makefile builds a Test Provider Namepspace
# ATTN: Today this makefile is implementation dependent.
#       It assumes that the repository is a set of directories
#       that can be removed by removing the directories.
#       This must be updated to an implementation independent repository
#       remove function.

ROOT = $(PEGASUS_ROOT)
include $(ROOT)/mak/config.mak
include $(ROOT)/mak/configschema.mak

# The following variable determine the version of the Schema to
# be loaded into the Test Provider namespace.
# Update the following variable to change the version.

CIM_SCHEMA_VER=25
CIM_SCHEMA_DIR=CIM$(CIM_SCHEMA_VER)
CIM_MOF_PATH = $(ROOT)/Schemas/$(CIM_SCHEMA_DIR)

TESTPROVIDERNS=root/TestProvider
TESTPROVIDERNSDIRECTORY=$(REPOSITORY_ROOT)/root\#TestProvider

repository:
	@ $(ECHO) +++++ Removing existing repository named $(TESTPROVIDERNS)
	@$(RMREPOSITORY) $(TESTPROVIDERNSDIRECTORY)
	@ $(ECHO) +++++ Removing existing repository named $(TESTPROVIDERNS)
	@$(RMREPOSITORY) $(TESTPROVIDERNSDIRECTORY)
	@ $(ECHO) +++++ Loading CIM_Core$(CIM_SCHEMA_VER) into $(TESTPROVIDERNS) namespace ...
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-I$(CIM_MOF_PATH)" "-n$(TESTPROVIDERNS)" $(CIM_MOF_PATH)/CIM_Core$(CIM_SCHEMA_VER).mof
	@ $(ECHO) +++++ Loading TestProvider class definitions into $(TESTPROVIDERNS) namespace ...
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-n$(TESTPROVIDERNS)" ClientProviderSchema.mof
	@ $(ECHO) +++++ Registering TestProvider schema Providers  ...
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-n$(INTEROPNS)" ClientProviderSchemaR.mof

repositoryServer:
	@ $(ECHO) +++++ Loading CIM_Core$(CIM_SCHEMA_VER) into $(TESTPROVIDERNS) namespace ...
	@ cimmof "-I$(CIM_MOF_PATH)" "-n$(TESTPROVIDERNS)" $(CIM_MOF_PATH)/CIM_Core$(CIM_SCHEMA_VER).mof
	@ cimmof "-n$(TESTPROVIDERNS)" ClientProviderSchema.mof
	@ cimmof "-n$(INTEROPNS)" ClientProviderSchemaR.mof

removerepository:
	@ $(ECHO) +++++ Removing existing repository named $(TESTPROVIDERNS)
	@ $(RMREPOSITORY) $(TESTPROVIDERNSDIRECTORY)

unregister:
	$(MAKE) -i unregisterproviders

unregisterproviders:
	cimprovider -r -m TestClientProviderModule

depend:

sub:

misc:

tests:

poststarttests:

general:

clean:
