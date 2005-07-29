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
# Makefile builds a TestProvider Namespace
# ATTN: Today this makefile is implementation dependent.
#       It assumes that the repository is a set of directories
#       that can be removed by removing the directories.
#       This must be updated to an implementation independent repository
#       remove function.
#       Note that this is, in fact, the RMREPOSITORY function
#       which we should be able to replace with an executable
#       making this probably implementation independent.

ROOT = $(PEGASUS_ROOT)
include $(ROOT)/mak/config.mak
include $(ROOT)/mak/configschema.mak

# Loads the sample MOF into a separate namespace.
# The following variable determine the version of the Schema to
# be loaded into the TestProvider namespace.
# Update the following variable to change the version.
# The following variables determine the Namespace into which the
# TestProvider classes are to be loaded.

TESTPROVIDERNS=test/TestProvider
TESTPROVIDERNSDIRECTORY=$(REPOSITORY_ROOT)/test\#TestProvider
##
## Clean the existing repository and load the test MOF with cimmofl.
##
repository:
	@ $(ECHO) +++++ Removing existing repository named $(TESTPROVIDERNS)
	@$(RMREPOSITORY) $(TESTPROVIDERNSDIRECTORY)
	@ $(ECHO) +++++ Loading Core$(CIM_SCHEMA_VER)_Qualifiers.mof into $(TESTPROVIDERNS) namespace ...
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" -n$(TESTPROVIDERNS) $(CIM_SCHEMA_DIR)/Core$(CIM_SCHEMA_VER)_Qualifiers.mof
	@ $(ECHO) +++++ Loading CIM_Core$(CIM_SCHEMA_VER) into $(TESTPROVIDERNS) namespace ...
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-I$(CIM_SCHEMA_DIR)" "-n$(TESTPROVIDERNS)" $(ALLOW_EXPERIMENTAL) $(CIM_SCHEMA_DIR)/CIM_Core$(CIM_SCHEMA_VER).mof
	@ $(ECHO) +++++ Loading CIM_Event$(CIM_SCHEMA_VER) into $(TESTPROVIDERNS) namespace ...
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-I$(CIM_SCHEMA_DIR)" "-n$(TESTPROVIDERNS)" $(ALLOW_EXPERIMENTAL) $(CIM_SCHEMA_DIR)/CIM_Event$(CIM_SCHEMA_VER).mof

	@ $(ECHO) +++++ Loading PG_Events$(PG_INTEROP_SCHEMA_VER) into $(TESTPROVIDERNS) namespace ...
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-I$(PG_INTEROP_MOF_PATH)" "-n$(TESTPROVIDERNS)" $(ALLOW_EXPERIMENTAL) $(PG_INTEROP_MOF_PATH)/PG_Events$(PG_INTEROP_SCHEMA_VER).mof
	@ $(ECHO) +++++ Loading TestInstanceProvider class definitions into $(TESTPROVIDERNS) namespace ...
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-n$(TESTPROVIDERNS)" TestInstanceProviderSchema.mof
	@ $(ECHO) +++++ Loading IndicationStressTest class definitions into $(TESTPROVIDERNS) namespace ...
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-n$(TESTPROVIDERNS)" IndicationStressTestProviderSchema.mof

	@ $(ECHO) +++++ Registering TestInstanceProvider ...
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-n$(INTEROPNS)" TestInstanceProviderRegistration.mof
	@ $(ECHO) +++++ Registering IndicationStressTest Provider  ...
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-n$(INTEROPNS)" IndicationStressTestProviderR.mof

	@ $(ECHO) +++++ Loading MethodTestProvider class definitions into $(TESTPROVIDERNS) namespace ...
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-n$(TESTPROVIDERNS)" MethodTestProvider.mof
	@ $(ECHO) +++++ Registering MethodTestProvider ...
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-n$(INTEROPNS)" MethodTestProviderR.mof

	@ $(ECHO) +++++ Installing the Family MOF file for the association provider
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-n$(TESTPROVIDERNS)" TST_Family.mof
	@ $(ECHO) +++++ Registering FamilyProvider schema Providers  ...
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-n$(INTEROPNS)" FamilyProviderSchemaR.mof

ifdef PEGASUS_ENABLE_CMPI_PROVIDER_MANAGER
	@ $(ECHO) +++++ Loading TestCMPI_ProviderSchema class definitions into $(TESTPROVIDERNS) namespace ...
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-n$(TESTPROVIDERNS)" TestCMPI_ProviderSchema.mof

	@ $(ECHO) +++++ Registering TestCMPI_ProviderRegistration ...
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-n$(INTEROPNS)" TestCMPI_ProviderRegistration.mof
ifdef PEGASUS_ENABLE_EXECQUERY
	@ $(ECHO) +++++ Registering TestCMPI_ProviderExecQueryRegistration ...
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-n$(INTEROPNS)" TestCMPI_ProviderExecQueryRegistration.mof
endif

endif

ifndef PEGASUS_DISABLE_PROV_USERCTXT
	@ $(ECHO) +++++ Loading UserContextTestProvider class definitions into $(TESTPROVIDERNS) namespace ...
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-n$(TESTPROVIDERNS)" UserContextTestProvider.mof

	@ $(ECHO) +++++ Registering UserContextTestProvider ...
ifndef PEGASUS_DISABLE_PROV_USERCTXT_REQUESTOR
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-n$(INTEROPNS)" UserContextRequestorR.mof
endif
ifndef PEGASUS_DISABLE_PROV_USERCTXT_PRIVILEGED
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-n$(INTEROPNS)" UserContextPrivilegedR.mof
endif
ifndef PEGASUS_DISABLE_PROV_USERCTXT_DESIGNATED
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-n$(INTEROPNS)" UserContextDesignatedR.mof
endif
ifndef PEGASUS_DISABLE_PROV_USERCTXT_CIMSERVER
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-n$(INTEROPNS)" UserContextCIMServerR.mof
endif
endif

##
## Load the test MOF with cimmof.  (Do not remove the existing repository.)
##
repositoryServer:
	@ $(ECHO) +++++ Loading Core$(CIM_SCHEMA_VER)_Qualifiers.mof into $(TESTPROVIDERNS) namespace ...
	@ cimmof -n$(TESTPROVIDERNS) $(CIM_SCHEMA_DIR)/Core$(CIM_SCHEMA_VER)_Qualifiers.mof
	@ $(ECHO) +++++ Loading CIM_Core$(CIM_SCHEMA_VER) into $(TESTPROVIDERNS) namespace ...
	@ cimmof "-I$(CIM_SCHEMA_DIR)" "-n$(TESTPROVIDERNS)" $(ALLOW_EXPERIMENTAL) $(CIM_SCHEMA_DIR)/CIM_Core$(CIM_SCHEMA_VER).mof
	@ $(ECHO) +++++ Loading CIM_Event$(CIM_SCHEMA_VER) into $(TESTPROVIDERNS) namespace ...
	@ cimmof "-I$(CIM_SCHEMA_DIR)" "-n$(TESTPROVIDERNS)" $(ALLOW_EXPERIMENTAL) $(CIM_SCHEMA_DIR)/CIM_Event$(CIM_SCHEMA_VER).mof
	@ $(ECHO) +++++ Loading PG_Events$(PG_INTEROP_SCHEMA_VER) into $(TESTPROVIDERNS) namespace ...
	@ cimmof "-I$(PG_INTEROP_MOF_PATH)" "-n$(TESTPROVIDERNS)" $(ALLOW_EXPERIMENTAL) $(PG_INTEROP_MOF_PATH)/PG_Events$(PG_INTEROP_SCHEMA_VER).mof
	@ $(ECHO) +++++ Loading IndicationStressTest class definition into $(TESTPROVIDERNS) namespace ...
	@ cimmof "-n$(TESTPROVIDERNS)" IndicationStressTestProviderSchema.mof
	@ $(ECHO) +++++ Registering IndicationStressTest Provider  ...
	@ cimmof "-n$(INTEROPNS)" IndicationStressTestProviderR.mof

	@ $(ECHO) +++++ Loading MethodTestProvider class definitions into $(TESTPROVIDERNS) namespace ...
	@ cimmof "-n$(TESTPROVIDERNS)" MethodTestProvider.mof
	@ $(ECHO) +++++ Registering MethodTestProvider ...
	@ cimmof "-n$(INTEROPNS)" MethodTestProviderR.mof

	@ $(ECHO) +++++ Installing the Family MOF file for the association provider
	@ cimmof "-n$(TESTPROVIDERNS)" TST_Family.mof
	@ $(ECHO) +++++ Registering FamilyProvider schema Providers  ...
	@ cimmof "-n$(INTEROPNS)" FamilyProviderSchemaR.mof

ifdef PEGASUS_ENABLE_CMPI_PROVIDER_MANAGER
	@ $(ECHO) +++++ Loading TestCMPI_ProviderSchema class definitions into $(TESTPROVIDERNS) namespace ...
	@ cimmof   "-n$(TESTPROVIDERNS)" TestCMPI_ProviderSchema.mof
	@ $(ECHO) +++++ Registering TestCMPI_ProviderRegistration ...
	@ cimmof   "-n$(INTEROPNS)" TestCMPI_ProviderRegistration.mof
ifdef PEGASUS_ENABLE_EXECQUERY
	@ $(ECHO) +++++ Registering TestCMPI_ProviderExecQueryRegistration ...
	@ cimmof   "-n$(INTEROPNS)" TestCMPI_ProviderExecQueryRegistration.mof
endif

endif

ifndef PEGASUS_DISABLE_PROV_USERCTXT
	@ $(ECHO) +++++ Loading UserContextTestProvider class definitions into $(TESTPROVIDERNS) namespace ...
	@ cimmof "-n$(TESTPROVIDERNS)" UserContextTestProvider.mof

	@ $(ECHO) +++++ Registering UserContextTestProvider ...
ifndef PEGASUS_DISABLE_PROV_USERCTXT_REQUESTOR
	@ cimmof "-n$(INTEROPNS)" UserContextRequestorR.mof
endif
ifndef PEGASUS_DISABLE_PROV_USERCTXT_PRIVILEGED
	@ cimmof "-n$(INTEROPNS)" UserContextPrivilegedR.mof
endif
ifndef PEGASUS_DISABLE_PROV_USERCTXT_DESIGNATED
	@ cimmof "-n$(INTEROPNS)" UserContextDesignatedR.mof
endif
ifndef PEGASUS_DISABLE_PROV_USERCTXT_CIMSERVER
	@ cimmof "-n$(INTEROPNS)" UserContextCIMServerR.mof
endif
endif

##
##
##
removerepository:
	@ $(ECHO) +++++ Removing existing repository named $(TESTPROVIDERNS)
	@ $(RMREPOSITORY) $(TESTPROVIDERNSDIRECTORY)


unregister:
	$(MAKE) -i unregisterproviders


unregisterproviders:
	cimprovider -r -m IndicationStressTestProviderModule
	cimprovider -r -m MethodTestProviderModule
ifdef PEGASUS_ENABLE_CMPI_PROVIDER_MANAGER
	cimprovider -r -m TestCMPIIndicationProviderModule
	cimprovider -r -m TestCMPIMethodProviderModule
	cimprovider -r -m TestCMPIFail_1ProviderModule
	cimprovider -r -m TestCMPIFail_2ProviderModule
	cimprovider -r -m TestCMPIFail_3ProviderModule 
	cimprovider -r -m TestCMPIFail_4ProviderModule 
ifdef PEGASUS_ENABLE_EXECQUERY
	cimprovider -r -m TestCMPIInstanceProviderModule
endif
endif 
ifndef PEGASUS_DISABLE_PROV_USERCTXT
ifndef PEGASUS_DISABLE_PROV_USERCTXT_REQUESTOR
	cimprovider -r -m UserContextRequestorModule
endif
ifndef PEGASUS_DISABLE_PROV_USERCTXT_PRIVILEGED
	cimprovider -r -m UserContextPrivilegedModule
endif
ifndef PEGASUS_DISABLE_PROV_USERCTXT_DESIGNATED
	cimprovider -r -m UserContextDesignatedModule
endif
ifndef PEGASUS_DISABLE_PROV_USERCTXT_CIMSERVER
	cimprovider -r -m UserContextCIMServerModule
endif
endif

depend:

sub:

misc:

tests:

poststarttests:

general:

clean:

