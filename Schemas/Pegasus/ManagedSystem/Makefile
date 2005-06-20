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
# Makefile builds a Managed System Namepspace 
# ATTN: Today this makefile is implementation dependent.
#       It assumes that the repository is a set of directories
#       that can be removed by removing the directories.
#       This must be updated to an implementation independent repository
#       remove function.

ROOT = $(PEGASUS_ROOT)
include $(ROOT)/mak/config.mak
include $(ROOT)/mak/configschema.mak

# The following two variables determine the version of the Schema to
# be loaded into the Managed System namespace.
# Update the following two environment variables to change the version.

PG_SCHEMA_VER=20
PG_SCHEMA_DIR=VER$(PG_SCHEMA_VER)
PG_MOF_PATH=$(ROOT)/Schemas/Pegasus/ManagedSystem/$(PG_SCHEMA_DIR)
FILE_PREFIX=PG
# The following defines are used to determine the version of 
# the platform specific schema to be loaded into Managed System namespace.

MANAGEDSYSTEM_NS_SUPPORTED = true

ifeq ($(OS), HPUX)
EXTENDED_MANAGEDSYSTEM_NS_SUPPORTED = true
FILE_PREFIX=HPUX
endif

ifeq ($(OS), VMS)
EXTENDED_MANAGEDSYSTEM_NS_SUPPORTED = true
FILE_PREFIX=HPUX
endif

ifeq ($(MANAGEDSYSTEM_NS_SUPPORTED),true)
repository:
	@ $(ECHO) +++++ Removing existing repository named $(MANAGEDSYSTEMNS)
	@ $(RMREPOSITORY) $(MANAGEDSYSTEMNSDIRECTORY)
	@ $(ECHO) +++++ Loading CIM_Schema$(CIM_SCHEMA_VER) into $(MANAGEDSYSTEMNS) namespace ...
	@ $(CIMMOFCLI) "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-I$(CIM_SCHEMA_DIR)" "-n$(MANAGEDSYSTEMNS)" $(ALLOW_EXPERIMENTAL) "$(CIM_SCHEMA_DIR)/CIM_Schema$(CIM_SCHEMA_VER).mof"
	@ $(ECHO) +++++ Loading PG_Events$(PG_INTEROP_SCHEMA_VER) into $(MANAGEDSYSTEMNS) namespace ...
	@ $(CIMMOFCLI) "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-I$(PG_INTEROP_MOF_PATH)" "-n$(MANAGEDSYSTEMNS)" "$(PG_INTEROP_MOF_PATH)/PG_Events$(PG_INTEROP_SCHEMA_VER).mof"
	@ $(ECHO) +++++ Loading PG_ManagedSystemSchema$(PG_SCHEMA_VER) into $(MANAGEDSYSTEMNS) namespace ...
	@ $(CIMMOFCLI) "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-I$(PG_MOF_PATH)" "-n$(MANAGEDSYSTEMNS)" "$(PG_MOF_PATH)/PG_ManagedSystemSchema$(PG_SCHEMA_VER).mof"
	@ $(ECHO) +++++ Registering Providers for PG_ManagedSystemSchema$(PG_SCHEMA_VER) ...
	@ $(CIMMOFCLI) "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-I$(PG_MOF_PATH)" "-n$(INTEROPNS)" "$(PG_MOF_PATH)/PG_ManagedSystemSchema$(PG_SCHEMA_VER)R.mof"
ifndef PEGASUS_USE_RELEASE_CONFIG_OPTIONS
	@ $(ECHO) +++++ Loading PG_ManagedSystemTestSchema$(PG_SCHEMA_VER) into $(MANAGEDSYSTEMNS) namespace ...
	@ $(CIMMOFCLI) "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-I$(PG_MOF_PATH)" "-n$(MANAGEDSYSTEMNS)" "$(PG_MOF_PATH)/PG_ManagedSystemTestSchema$(PG_SCHEMA_VER).mof"
	@ $(ECHO) +++++ Registering Providers for PG_ManagedSystemTestSchema$(PG_SCHEMA_VER) ...
	@ $(CIMMOFCLI) "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-I$(PG_MOF_PATH)" "-n$(INTEROPNS)" "$(PG_MOF_PATH)/PG_ManagedSystemTestSchema$(PG_SCHEMA_VER)R.mof"
endif
ifeq ($(EXTENDED_MANAGEDSYSTEM_NS_SUPPORTED),true)
	@ $(ECHO) +++++ Loading platform specific class definitions in $(MANAGEDSYSTEMNS) namespace  ...
	@ $(CIMMOFCLI) "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-I$(PG_MOF_PATH)" "-n$(MANAGEDSYSTEMNS)" "$(PG_MOF_PATH)/$(FILE_PREFIX)_ManagedSystemSchema$(PG_SCHEMA_VER).mof"
	@ $(ECHO) +++++ Registering Providers for platform specific classes in $(MANAGEDSYSTEMNS) namespace ...
	@ $(CIMMOFCLI) "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-I$(PG_MOF_PATH)" "-n$(INTEROPNS)" "$(PG_MOF_PATH)/$(FILE_PREFIX)_ManagedSystemSchema$(PG_SCHEMA_VER)R.mof"
endif
ifeq ($(PEGASUS_ENABLE_SLP),true)	
	@ $(ECHO) +++++ Registering Providers for platform specific classes in $(MANAGEDSYSTEMNS) namespace ...
	@ $(CIMMOFCLI) "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-I$(PG_MOF_PATH)" "-n$(INTEROPNS)" "$(PG_MOF_PATH)/$(FILE_PREFIX)_SLPProvider$(PG_SCHEMA_VER)R.mof"
endif

repositoryServer:
	@ $(ECHO) +++++ Loading CIM_Schema$(CIM_SCHEMA_VER) into $(MANAGEDSYSTEMNS) namespace ...
	@ cimmof "-I$(CIM_SCHEMA_DIR)" "-n$(MANAGEDSYSTEMNS)" $(ALLOW_EXPERIMENTAL) "$(CIM_SCHEMA_DIR)/CIM_Schema$(CIM_SCHEMA_VER).mof"
	@ $(ECHO) +++++ Loading PG_Events$(PG_INTEROP_SCHEMA_VER) into $(MANAGEDSYSTEMNS) namespace ...
	@ cimmof "-I$(PG_INTEROP_MOF_PATH)" "-n$(MANAGEDSYSTEMNS)" "$(PG_INTEROP_MOF_PATH)/PG_Events$(PG_INTEROP_SCHEMA_VER).mof"
	@ $(ECHO) +++++ Loading PG_ManagedSystemSchema$(PG_SCHEMA_VER) into $(MANAGEDSYSTEMNS) namespace ...
	@ cimmof  "-I$(PG_MOF_PATH)" "-n$(MANAGEDSYSTEMNS)" "$(PG_MOF_PATH)/PG_ManagedSystemSchema$(PG_SCHEMA_VER).mof"
	@ $(ECHO) +++++ Registering Providers for PG_ManagedSystemSchema$(PG_SCHEMA_VER) ...
	@ cimmof "-I$(PG_MOF_PATH)" "-n$(INTEROPNS)" "$(PG_MOF_PATH)/PG_ManagedSystemSchema$(PG_SCHEMA_VER)R.mof"
ifndef PEGASUS_USE_RELEASE_CONFIG_OPTIONS
	@ $(ECHO) +++++ Loading PG_ManagedSystemTestSchema$(PG_SCHEMA_VER) into $(MANAGEDSYSTEMNS) namespace ...
	@ cimmof  "-I$(PG_MOF_PATH)" "-n$(MANAGEDSYSTEMNS)" "$(PG_MOF_PATH)/PG_ManagedSystemTestSchema$(PG_SCHEMA_VER).mof"
	@ $(ECHO) +++++ Registering Providers for PG_ManagedSystemTestSchema$(PG_SCHEMA_VER) ...
	@ cimmof "-I$(PG_MOF_PATH)" "-n$(INTEROPNS)" "$(PG_MOF_PATH)/PG_ManagedSystemTestSchema$(PG_SCHEMA_VER)R.mof"
endif
ifeq ($(EXTENDED_MANAGEDSYSTEM_NS_SUPPORTED),true)
	@ $(ECHO) +++++ Loading platform specific class definitions in $(MANAGEDSYSTEMNS) namespace  ...
	@ cimmof "-I$(PG_MOF_PATH)" "-n$(MANAGEDSYSTEMNS)" "$(PG_MOF_PATH)/$(FILE_PREFIX)_ManagedSystemSchema$(PG_SCHEMA_VER).mof"
	@ $(ECHO) +++++ Registering Providers for platform specific classes in $(MANAGEDSYSTEMNS) namespace ...
	@ cimmof "-I$(PG_MOF_PATH)" "-n$(INTEROPNS)" "$(PG_MOF_PATH)/$(FILE_PREFIX)_ManagedSystemSchema$(PG_SCHEMA_VER)R.mof"
endif
ifeq ($(PEGASUS_ENABLE_SLP),true) 
	@ $(ECHO) +++++ Registering Providers for platform specific classes in $(MANAGEDSYSTEMNS) namespace ...
	@ cimmof "-I$(PG_MOF_PATH)" "-n$(INTEROPNS)" "$(PG_MOF_PATH)/$(FILE_PREFIX)_SLPProvider$(PG_SCHEMA_VER)R.mof"
endif

clean:
	@ $(ECHO) +++++ Removing existing Repository named $(MANAGEDSYSTEMNS)
	@ $(RMREPOSITORY) $(MANAGEDSYSTEMNSDIRECTORY)
else
repository:
	@ $(ECHO) +++++ No Managed System Namespace defined for this platform ++++

clean:

endif


depend:

sub:

misc:

tests:

poststarttests:

general:
