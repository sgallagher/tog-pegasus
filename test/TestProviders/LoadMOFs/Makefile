# Copyright (c) 2004 IBM Corporation.
# 
# Author: Subodh Soni (ssubodh@in.ibm.com)
#
# Makefile for loading for all the MOF files for the test enhancement providers
# present in this directory. It creates a new namespace called SampleProvider/
# inside $PEGASUS_HOME/repository.
# The implementation of sample providers and classes registered by this 
# Makefile are present in the following directories:
#
# 1. ExceptionsTest/
# 2. CIMOMSample/
# 3. LargeDataProvider/
# 4. MultithreadingTests/
# 5. TimingTests/
# 6. XMLParserGen/
#

ROOT = $(PEGASUS_ROOT)

include $(ROOT)/mak/config.mak
include $(ROOT)/mak/configschema.mak

# Loads the sample MOF into a separate namespace.
# The following variable determine the version of the Schema to be loaded into 
# the Sample Provider namespace. Update this to change the Schema version
#CIM_SCHEMA_DIR=///

SAMPLEPROVIDERNS=root/SampleProvider
SAMPLEPROVIDERNSDIRECTORY=$(REPOSITORY_ROOT)/root\#SampleProvider

# Load the sample MOF with the local compiler. Cleans the existing repository
# before reloading.

repository:
	@ echo +++++ Removing existing repository named $(SAMPLEPROVIDERNS)
	@$(RMREPOSITORY) $(SAMPLEPROVIDERNSDIRECTORY)
	@ echo +++++ Loading CIM_Core$(CIM_SCHEMA_VER) into $(SAMPLEPROVIDERNS) namespace ...
	@ cimmofl -R$(REPOSITORY_DIR) -I$(CIM_SCHEMA_DIR) -n$(SAMPLEPROVIDERNS) $(ALLOW_EXPERIMENTAL) $(CIM_SCHEMA_DIR)/CIM_Core$(CIM_SCHEMA_VER).mof
	@ echo +++++ Loading CIM_Event$(CIM_SCHEMA_VER) into $(SAMPLEPROVIDERNS) namespace ...
	@ cimmofl -R$(REPOSITORY_DIR) -I$(CIM_SCHEMA_DIR) -n$(SAMPLEPROVIDERNS) $(ALLOW_EXPERIMENTAL) $(CIM_SCHEMA_DIR)/CIM_Event$(CIM_SCHEMA_VER).mof

	@ echo +++++ Loading All Sample Providers class definitions into $(SAMPLEPROVIDERNS) namespace ...
	@ cimmofl -R$(REPOSITORY_DIR) -n$(SAMPLEPROVIDERNS)	CIMOMSampleProvider.mof
	@ cimmofl -R$(REPOSITORY_DIR) -n$(SAMPLEPROVIDERNS) LargeDataProvider.mof
	@ cimmofl -R$(REPOSITORY_DIR) -n$(SAMPLEPROVIDERNS) MultithreadingSampleProvider.mof
	@ cimmofl -R$(REPOSITORY_DIR) -n$(SAMPLEPROVIDERNS) ExceptionsTest.mof
	@ cimmofl -R$(REPOSITORY_DIR) -n$(SAMPLEPROVIDERNS) TimingProvider.mof

	@ echo +++++ Registering All Sample Providers ....
	@ cimmofl -R$(REPOSITORY_DIR) -n$(INTEROPNS) CIMOMSampleProviderR.mof
	@ cimmofl -R$(REPOSITORY_DIR) -n$(INTEROPNS) LargeDataProviderR.mof
	@ cimmofl -R$(REPOSITORY_DIR) -n$(INTEROPNS) MultithreadingSampleProviderR.mof
	@ cimmofl -R$(REPOSITORY_DIR) -n$(INTEROPNS) ExceptionsTestR.mof
	@ cimmofl -R$(REPOSITORY_DIR) -n$(INTEROPNS) TimingProviderR.mof

# Load the sample mofe through the Client interface compiler but do not
# clean out the existing version.
repositoryServer:
	@ echo +++++ Loading CIM_Core$(CIM_SCHEMA_VER) into $(SAMPLEPROVIDERNS) namespace ...
	@ cimmof -I$(CIM_SCHEMA_DIR) -n$(SAMPLEPROVIDERNS) $(ALLOW_EXPERIMENTAL) $(CIM_SCHEMA_DIR)/CIM_Core$(CIM_SCHEMA_VER).mof
	@ echo +++++ Loading CIM_Event$(CIM_SCHEMA_VER) into $(SAMPLEPROVIDERNS) namespace ...
	@ cimmof -I$(CIM_SCHEMA_DIR) -n$(SAMPLEPROVIDERNS) $(ALLOW_EXPERIMENTAL) $(CIM_SCHEMA_DIR)/CIM_Event$(CIM_SCHEMA_VER).mof
	@ echo +++++ Loading All the Sample Providers class definitions into $(SAMPLEPROVIDERNS) namespace ...
	@ cimmof -n$(SAMPLEPROVIDERNS) CIMOMSampleProvider.mof
	@ cimmof -n$(SAMPLEPROVIDERNS) LargeDataProvider.mof
	@ cimmof -n$(SAMPLEPROVIDERNS) MultithreadingSampleProvider.mof
	@ cimmof -n$(SAMPLEPROVIDERNS) ExceptionsTest.mof
	@ cimmof -n$(SAMPLEPROVIDERNS) TimingProvider.mof

	@ echo +++++ Registering All Sample Providers Schema Providers  ...
	@ cimmof -n$(INTEROPNS)	CIMOMSampleProviderR.mof
	@ cimmof -n$(INTEROPNS) LargeDataProviderR.mof
	@ cimmof -n$(INTEROPNS) MultithreadingSampleProviderR.mof
	@ cimmof -n$(INTEROPNS) ExceptionsTestR.mof
	@ cimmof -n$(INTEROPNS) TimingProviderR.mof

removerepository:
	@ echo +++++ Removing existing repository named $(SAMPLEPROVIDERNS)
	@ $(RMREPOSITORY) $(SAMPLEPROVIDERNSDIRECTORY)

unregister:
	$(MAKE) -i unregisterproviders

unregisterproviders:
	cimprovider -r -m ExceptionsTestModule
	cimprovider -r -m LargeDataProviderModule
	cimprovider -r -m CIMOMSampleProviderModule
	cimprovider -r -m MultithreadingSampleProviderModule
	cimprovider -r -m TimingProviderModule

depend:

sub:

misc:

tests:

messages:

poststarttests:

general:

clean:
