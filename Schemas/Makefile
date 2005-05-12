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
# Makefile builds the Pegasus Repository
# ATTN: Today this makefile is implementation dependent.
#       It assumes that the repository is a set of directories
#       that can be removed by removing the directories.
#       This must be updated to an implementation independent repository
#       remove function.

ROOT = $(PEGASUS_ROOT)
include $(ROOT)/mak/config.mak
include $(ROOT)/mak/configschema.mak

# Define Namespace in which the CIM Schemas are to be Loaded.
# Note that by default, the CORE is also loaded into root.

NAMESPACE=root/cimv2

all:

messages:

# Delete and rebuild the repository using the MOF compiler.

repository:
	@ $(ECHO) +++++ Removing existing repository named $(REPOSITORY_ROOT)
	@ $(RMREPOSITORY) $(REPOSITORY_ROOT)
	@ $(ECHO) +++++ Populating CIM repository - this will take a while
	@ $(ECHO) +++++ Loading CIM_Core$(CIM_SCHEMA_VER).mof into root namespace...
	@ $(CIMMOFCLI) "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-I$(CIM_SCHEMA_DIR)" -nroot $(ALLOW_EXPERIMENTAL) $(CIM_SCHEMA_DIR)/CIM_Core$(CIM_SCHEMA_VER).mof
	@ $(ECHO) +++++ Loading CIM_Schema$(CIM_SCHEMA_VER).mof into $(NAMESPACE) namespace...
	@ $(CIMMOFCLI) "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-I$(CIM_SCHEMA_DIR)" "-n$(NAMESPACE)" $(ALLOW_EXPERIMENTAL) $(CIM_SCHEMA_DIR)/CIM_Schema$(CIM_SCHEMA_VER).mof

# The following is a hangover from previous testing and simply loads the
# core into the namespace defined by NAMESPACE.  It should probably be 
# deleted since there is no use for it any more.
short:
	@ $(ECHO) +++++ Populating CIM repository - this will take a minute
	@ $(RMREPOSITORY) $(REPOSITORY_ROOT)
	@ $(ECHO) +++++ Loading CIM_Core.mof into root namespace...
	@ $(CIMMOFCLI) "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-I$(CIM_SCHEMA_DIR)" "-n$(NAMESPACE)" $(ALLOW_EXPERIMENTAL) $(CIM_SCHEMA_DIR)/CIM_Core$(CIM_SCHEMA_VER).mof

clean:
	@ $(ECHO) +++++ Removing existing CIM Repository named $(REPOSITORY_ROOT)
	@ $(RMREPOSITORY) $(REPOSITORY_ROOT)


depend:

sub:

misc:

tests:

poststarttests:

general:

# used to generate xml output from the schema

xml:
	@ $(RMREPOSITORY) $(TMP_DIR)/repository
	@ $(ECHO) +++++ Temporarily loading CIM_Core$(CIM_SCHEMA_VER) into xml/cimv2 namespace ...
	$(CIMMOFCLI) "-R$(TMP_DIR)" "-I$(CIM_SCHEMA_DIR)" -nxml/cimv2 $(CIM_SCHEMA_DIR)/CIM_Core$(CIM_SCHEMA_VER).mof
	@ $(ECHO) +++++ Creating XML for CIM Schema$(CIM_SCHEMA_VER).mof in file $(TMP_DIR)/CIM_Schema$(CIM_SCHEMA_VER).xml
	$(CIMMOFCLI)  "-R$(TMP_DIR)" --xml "-I$(CIM_SCHEMA_DIR)" -nxml/cimv2 $(CIM_SCHEMA_DIR)/CIM_Schema$(CIM_SCHEMA_VER).mof > $(TMP_DIR)/CIM_Schema$(CIM_SCHEMA_VER).xml
	@ $(RMREPOSITORY) $(TMP_DIR)/repository
