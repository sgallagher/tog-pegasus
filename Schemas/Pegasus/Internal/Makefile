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
# Makefile builds the Pegasus Internal Namepspace
# ATTN: Today this makefile is implementation dependent.
#       It assumes that the repository is a set of directories
#       that can be removed by removing the directories.
#       This must be updated to an implementation independent repository
#       remove function.

ROOT = $(PEGASUS_ROOT)
include $(ROOT)/mak/config.mak
include $(ROOT)/mak/configschema.mak

# The following two variables determine the version of the Schema to
# be loaded into the Pegasus Internal Namespace.
# Update the following two environment variables to change the version.

PG_SCHEMA_VER=20
PG_SCHEMA_DIR=VER$(PG_SCHEMA_VER)
PG_MOF_PATH=$(ROOT)/Schemas/Pegasus/Internal/$(PG_SCHEMA_DIR)

all:

# Delete and rebuild the repository using the MOF compiler.

repository:
	@ $(ECHO) +++++ Removing existing repository named $(INTERNALNS)
	@ $(RMREPOSITORY) $(INTERNALNSDIRECTORY)
	@ $(ECHO) +++++ Loading Core$(CIM_SCHEMA_VER)_Qualifiers.mof into $(INTERNALNS) namespace ...
	@ $(CIMMOFCLI) "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-n$(INTERNALNS)" "$(CIM_SCHEMA_DIR)/Core$(CIM_SCHEMA_VER)_Qualifiers.mof"
	@ $(ECHO) +++++ Loading PG_InternalSchema$(PG_SCHEMA_VER).mof into $(INTERNALNS) namespace...
	@ $(CIMMOFCLI) "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-I$(PG_MOF_PATH)" "-n$(INTERNALNS)" "$(PG_MOF_PATH)/PG_InternalSchema$(PG_SCHEMA_VER).mof"
ifdef PEGASUS_ENABLE_SLP
	@ $(ECHO) +++++ Loading PG_SLPTemplate.mof into $(INTERNALNS) namespace...
	@ $(CIMMOFCLI) "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-I$(PG_MOF_PATH)" "-n$(INTERNALNS)" "$(PG_MOF_PATH)/PG_SLPTemplate.mof"
endif


repositoryServer:
	@ $(ECHO) +++++ Loading Core$(CIM_SCHEMA_VER)_Qualifiers.mof into $(INTERNALNS) namespace ...
	@ cimmof "-n$(INTERNALNS)" "$(CIM_SCHEMA_DIR)/Core$(CIM_SCHEMA_VER)_Qualifiers.mof"
	@ $(ECHO) +++++ Loading PG_InternalSchema$(PG_SCHEMA_VER).mof into $(INTERNALNS) namespace...
	@ cimmof "-I$(PG_MOF_PATH)" "-n$(INTERNALNS)" "$(PG_MOF_PATH)/PG_InternalSchema$(PG_SCHEMA_VER).mof"
ifdef PEGASUS_ENABLE_SLP
	@ $(ECHO) +++++ Loading PG_SLPTemplate.mof into $(INTERNALNS) namespace...
	@ cimmof "-I$(PG_MOF_PATH)" "-n$(INTERNALNS)" "$(PG_MOF_PATH)/PG_SLPTemplate.mof"
endif

clean:
	@ $(ECHO) +++++ Removing existing CIM Repository named $(REPOSITORY_ROOT)
	@ $(RMREPOSITORY) $(INTERNALNSDIRECTORY)


depend:

sub:

misc:

tests:

poststarttests:

general:
