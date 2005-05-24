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
# Makefile builds a test namespace in the local directory.
# ATTN: Today this makefile is implementation dependent.
#       It assumes that the repository is a set of directories
#       that can be removed by removing the directories.
#       This must be updated to an implementation independent repository
#       remove function.

ROOT = $(PEGASUS_ROOT)
include $(ROOT)/mak/config.mak

REPOSITORYDIR = $(TMP_DIR)
NAMESPACENAME = root/PG_InterOp
NAMESPACEDIR = $(REPOSITORYDIR)/repository/root\#PG_InterOp

# Update the following two environment variables to change the version
# of the PG_Schema to be loaded in the namespace.

PG_SCHEMA_VER=20
PG_SCHEMA_DIR=VER$(PG_SCHEMA_VER)
PG_MOF_PATH = $(ROOT)/Schemas/Pegasus/InterOp/$(PG_SCHEMA_DIR)

all:

# Delete and rebuild the repository using the MOF compiler.

localrep:
	@ $(ECHO) +++++ Removing existing repository named $(NAMESPACEDIR)
	@ $(RMREPOSITORY) $(NAMESPACEDIR)
	@ $(ECHO) +++++ Loading CIM_Core$(CIM_SCHEMA_VER) into $(NAMESPACEDIR) namespace ...
	@ cimmofl "-I$(CIM_SCHEMA_DIR)" "-R$(REPOSITORYDIR)" "-M$(REPOSITORY_MODE)" "-n$(NAMESPACENAME)" $(ALLOW_EXPERIMENTAL) $(CIM_SCHEMA_DIR)/CIM_Core$(CIM_SCHEMA_VER).mof
	@ $(ECHO) +++++ Loading PG_ProviderModule$(PG_SCHEMA_VER) into $(NAMESPACEDIR) namespace ...
	@ cimmofl "-I$(PG_MOF_PATH)" "-R$(REPOSITORYDIR)" "-M$(REPOSITORY_MODE)" "-n$(NAMESPACENAME)" $(PG_MOF_PATH)/PG_ProviderModule$(PG_SCHEMA_VER).mof

clean:
	@ $(ECHO) +++++ Removing existing CIM Repository named $(NAMESPACEDIR)
	@ $(RMREPOSITORY) $(NAMESPACEDIR)

depend:

sub:

misc:

tests:

poststarttests:

general:
