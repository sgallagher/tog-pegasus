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
# Makefile builds a test cimv2 namespace
# ATTN: Today this makefile is implementation dependent.
#       It assumes that the repository is a set of directories
#       that can be removed by removing the directories.
#       This must be updated to an implementation independent repository
#       remove function.

ROOT = $(PEGASUS_ROOT)
include $(ROOT)/mak/config.mak
include $(ROOT)/mak/configschema.mak

# The following variable determine the version of the Schema to
# be loaded into the test cimv2 namespace.
# Update the following variable to change the version.

TESTCIMV2NS=test/cimv2
TESTCIMV2NSDIRECTORY=$(REPOSITORY_ROOT)/test\#cimv2

CIM_SCHEMA_VER=25
CIM_SCHEMA_DIR=$(PEGASUS_ROOT)/Schemas/CIM25

# ***** CIM_SCHEMA_DIR INFO ****
# If CIM_SCHEMA_DIR changes to use a preliminary schema which
# has experimentals make sure and change the path below to appopriate
# directory path.  Example:  CIMPrelim271 is preliminary and has
# experimental classes.  Since experimental classes exist the -aE
# option of the mof compiler needs to be set.
# *****
ifeq ($(CIM_SCHEMA_DIR), $(PEGASUS_ROOT)/Schemas/CIMPrelim271)
ALLOW_EXPERIMENTAL = "-aE"
else
ALLOW_EXPERIMENTAL =
endif

repository:
	@ $(ECHO) +++++ Removing existing repository named $(TESTCIMV2NS)
	@$(RMREPOSITORY) $(TESTCIMV2NSDIRECTORY)
	@ $(ECHO) +++++ Loading CIM_Schema$(CIM_SCHEMA_VER) into $(TESTCIMV2NS) namespace ...
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-I$(CIM_SCHEMA_DIR)" -n$(TESTCIMV2NS) $(ALLOW_EXPERIMENTAL) $(CIM_SCHEMA_DIR)/CIM_Schema$(CIM_SCHEMA_VER).mof

repositoryServer:
	@ $(ECHO) +++++ Loading CIM_Schema$(CIM_SCHEMA_VER) into $(TESTCIMV2NS) namespace ...
	@ cimmof "-I$(CIM_SCHEMA_DIR)" -n$(TESTCIMV2NS) $(ALLOW_EXPERIMENTAL) $(CIM_SCHEMA_DIR)/CIM_Schema$(CIM_SCHEMA_VER).mof

removerepository:
	@ $(ECHO) +++++ Removing existing repository named $(TESTCIMV2NS)
	@$(RMREPOSITORY) $(TESTCIMV2NSDIRECTORY)

clean:

depend:

sub:

misc:

tests:

messages:

poststarttests:

general:
