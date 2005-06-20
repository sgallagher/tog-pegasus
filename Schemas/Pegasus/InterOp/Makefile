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
# Makefile builds the Pegasus InterOp Namepspace
# ATTN: Today this makefile is implementation dependent.
#       It assumes that the repository is a set of directories
#       that can be removed by removing the directories.
#       This must be updated to an implementation independent repository
#       remove function.

ROOT = $(PEGASUS_ROOT)
include $(ROOT)/mak/config.mak
include $(ROOT)/mak/configschema.mak

# Create a single variable to control schema compile
ifdef PEGASUS_ENABLE_SLP
	PEGASUS_BUILD_INTEROP=TRUE
endif
ifndef PEGASUS_DISABLE_PERFINST
	PEGASUS_BUILD_INTEROP=TRUE
endif
# The following two variables determine the version of the Schema to
# be loaded into the Pegasus InterOp Namespace.
# Update the following two environment variables to change the version.

all:

# Delete and rebuild the repository using the MOF compiler.

repository:
	@$(ECHO) +++++ Removing existing repository named $(INTEROPNS)
	@$(RMREPOSITORY) $(INTEROPNSDIRECTORY)
	@$(ECHO) +++++ Loading CIM_Core$(CIM_SCHEMA_VER) into $(INTEROPNS) namespace ...
	@$(CIMMOFCLI) "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-I$(CIM_SCHEMA_DIR)" "-n$(INTEROPNS)" $(ALLOW_EXPERIMENTAL) "$(CIM_SCHEMA_DIR)/CIM_Core$(CIM_SCHEMA_VER).mof"
	@$(ECHO) +++++ Loading CIM_Event$(CIM_SCHEMA_VER) into $(INTEROPNS) namespace ...
	@$(CIMMOFCLI) "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-I$(CIM_SCHEMA_DIR)" "-n$(INTEROPNS)" $(ALLOW_EXPERIMENTAL) "$(CIM_SCHEMA_DIR)/CIM_Event$(CIM_SCHEMA_VER).mof"
#
ifdef PEGASUS_BUILD_INTEROP 
	@$(ECHO) +++++ Loading CIM_Interop$(CIM_SCHEMA_VER) into $(INTEROPNS) namespace ...
	@$(CIMMOFCLI) "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-I$(CIM_SCHEMA_DIR)" "-n$(INTEROPNS)" $(ALLOW_EXPERIMENTAL) "$(CIM_SCHEMA_DIR)/CIM_Interop$(CIM_SCHEMA_VER).mof" 
endif
#
	@$(ECHO) +++++ Loading PG_InterOpSchema$(PG_INTEROP_SCHEMA_VER).mof into $(INTEROPNS) namespace...
	@$(CIMMOFCLI) "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-I$(PG_INTEROP_MOF_PATH)" "-n$(INTEROPNS)" "$(PG_INTEROP_MOF_PATH)/PG_InterOpSchema$(PG_INTEROP_SCHEMA_VER).mof"
#
# Bug 1222
# The following code is temporary.  These classes should be incorporated into PG_InteropSchema
# however, this cannot be done now because we cannot conditionally compile MOF.	
ifdef PEGASUS_BUILD_INTEROP 
	@$(ECHO) +++++ Loading PG_CIMXMLCommunicationMechanism$(PG_INTEROP_SCHEMA_VER) into $(INTEROPNS) namespace ...
	@$(CIMMOFCLI) "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-I$(PG_INTEROP_MOF_PATH)" "-n$(INTEROPNS)" $(ALLOW_EXPERIMENTAL) "$(PG_INTEROP_MOF_PATH)/PG_CIMXMLCommunicationMechanism$(PG_INTEROP_SCHEMA_VER).mof" 
	@$(ECHO) +++++ Loading PG_Namespace$(PG_INTEROP_SCHEMA_VER) into $(INTEROPNS) namespace ...
	@$(CIMMOFCLI) "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-I$(PG_INTEROP_MOF_PATH)" "-n$(INTEROPNS)" $(ALLOW_EXPERIMENTAL) "$(PG_INTEROP_MOF_PATH)/PG_Namespace$(PG_INTEROP_SCHEMA_VER).mof"
endif

repositoryServer:
	@$(ECHO) +++++ Loading CIM_Core$(CIM_SCHEMA_VER) into $(INTEROPNS) namespace ...
	@cimmof "-I$(CIM_SCHEMA_DIR)" "-n$(INTEROPNS)" $(ALLOW_EXPERIMENTAL) "$(CIM_SCHEMA_DIR)/CIM_Core$(CIM_SCHEMA_VER).mof"
	@$(ECHO) +++++ Loading CIM_Event$(CIM_SCHEMA_VER) into $(INTEROPNS) namespace ...
	@cimmof "-I$(CIM_SCHEMA_DIR)" "-n$(INTEROPNS)" $(ALLOW_EXPERIMENTAL) "$(CIM_SCHEMA_DIR)/CIM_Event$(CIM_SCHEMA_VER).mof"

ifdef PEGASUS_BUILD_INTEROP 
	@$(ECHO) +++++ Loading CIM_Interop$(CIM_SCHEMA_VER) into $(INTEROPNS) namespace ...
	@cimmof "-I$(CIM_SCHEMA_DIR)" "-n$(INTEROPNS)" $(ALLOW_EXPERIMENTAL) "$(CIM_SCHEMA_DIR)/CIM_Interop$(CIM_SCHEMA_VER).mof"
endif

	@$(ECHO) +++++ Loading PG_InterOpSchema$(PG_INTEROP_SCHEMA_VER).mof into $(INTEROPNS) namespace...
	@cimmof "-I$(PG_INTEROP_MOF_PATH)" "-n$(INTEROPNS)" "$(PG_INTEROP_MOF_PATH)/PG_InterOpSchema$(PG_INTEROP_SCHEMA_VER).mof"
#
# Bug 1222
# The following code is temporary.  Eventually this class should be incorporated into PG_InteropSchema
# however, this cannot be done now because we cannot conditionally compile MOF.	
ifdef PEGASUS_BUILD_INTEROP 
	@$(ECHO) +++++ Loading Loading PG_CIMXMLCommunicationMechanism$(PG_INTEROP_SCHEMA_VER) into $(INTEROPNS) namespace ...
	@cimmof "-I$(PG_INTEROP_MOF_PATH)" "-n$(INTEROPNS)" $(ALLOW_EXPERIMENTAL) "$(PG_INTEROP_MOF_PATH)/PG_CIMXMLCommunicationMechanism$(PG_INTEROP_SCHEMA_VER).mof"
	@$(ECHO) +++++ Loading Loading PG_Namespace$(PG_INTEROP_SCHEMA_VER) into $(INTEROPNS) namespace ...
	@cimmof "-I$(PG_INTEROP_MOF_PATH)" "-n$(INTEROPNS)" $(ALLOW_EXPERIMENTAL) "$(PG_INTEROP_MOF_PATH)/PG_Namespace$(PG_INTEROP_SCHEMA_VER).mof"
endif

clean:
	@$(ECHO) +++++ Removing existing CIM Repository named $(INTEROPNS)
	@$(RMREPOSITORY) "$(INTEROPNSDIRECTORY)"


depend:

sub:

misc:

tests:

poststarttests:

general:
