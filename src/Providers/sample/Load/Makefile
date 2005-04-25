# Makefile builds a Sample Provider Namespace
# ATTN: Today this makefile is implementation dependent.
#       It assumes that the repository is a set of directories
#       that can be removed by removing the directories.
#       This must be updated to an implementation independent repository
#       remove function.

ROOT = $(PEGASUS_ROOT)
include $(ROOT)/mak/config.mak
include $(ROOT)/mak/configschema.mak

# Loads the sample MOF into a separate namespace.
# The following variable determine the version of the Schema to
# be loaded into the Sample Provider namespace.
# Update the following variable to change the version.

SAMPLEPROVIDERNS=root/SampleProvider
SAMPLEPROVIDERNSDIRECTORY=$(REPOSITORY_ROOT)/root\#SampleProvider

# The RT_Indication class definition must also be loaded into another namespace
# for testing
CIMV2NS=root/cimv2

# Load the sample MOF with the local compiler. Cleans the existing repository
# before reloading.
# NOTE: I believe that the remove is duplicated for some reason.
repository:
	@ $(ECHO) +++++ Removing existing repository named $(SAMPLEPROVIDERNS)
	@$(RMREPOSITORY) $(SAMPLEPROVIDERNSDIRECTORY)
	@ $(ECHO) +++++ Loading CIM_Core$(CIM_SCHEMA_VER) into $(SAMPLEPROVIDERNS) namespace ...
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-I$(CIM_SCHEMA_DIR)" "-n$(SAMPLEPROVIDERNS)" $(ALLOW_EXPERIMENTAL) $(CIM_SCHEMA_DIR)/CIM_Core$(CIM_SCHEMA_VER).mof
	@ $(ECHO) +++++ Loading CIM_Event$(CIM_SCHEMA_VER) into $(SAMPLEPROVIDERNS) namespace ...
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-I$(CIM_SCHEMA_DIR)" "-n$(SAMPLEPROVIDERNS)" $(ALLOW_EXPERIMENTAL) $(CIM_SCHEMA_DIR)/CIM_Event$(CIM_SCHEMA_VER).mof
	@ $(ECHO) +++++ Loading CIM_Physical$(CIM_SCHEMA_VER) into $(SAMPLEPROVIDERNS) namespace ...
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-I$(CIM_SCHEMA_DIR)" "-n$(SAMPLEPROVIDERNS)" $(ALLOW_EXPERIMENTAL) $(CIM_SCHEMA_DIR)/CIM_Physical$(CIM_SCHEMA_VER).mof
	@ $(ECHO) +++++ Loading CIM_System$(CIM_SCHEMA_VER) into $(SAMPLEPROVIDERNS) namespace ...
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-I$(CIM_SCHEMA_DIR)" "-n$(SAMPLEPROVIDERNS)" $(ALLOW_EXPERIMENTAL) $(CIM_SCHEMA_DIR)/CIM_System$(CIM_SCHEMA_VER).mof
	@ $(ECHO) +++++ Loading SampleProvider class definitions into $(SAMPLEPROVIDERNS) namespace ...
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-n$(SAMPLEPROVIDERNS)" SampleProviderSchema.mof
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-n$(SAMPLEPROVIDERNS)" AssociationProvider.mof
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-n$(SAMPLEPROVIDERNS)" RT_Indication.mof
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-n$(CIMV2NS)" RT_Indication.mof
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-n$(SAMPLEPROVIDERNS)" ClientTest.mof
	@ $(ECHO) +++++ Registering Sample Providers  ...
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-n$(INTEROPNS)" SampleProviderSchemaR.mof
ifdef PEGASUS_ENABLE_EXECQUERY
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-n$(INTEROPNS)" InstanceQueryProviderR.mof
endif

ifdef PEGASUS_ENABLE_CMPI_PROVIDER_MANAGER
	@ $(ECHO) +++++ Installing the MOF file for CMPI Providers
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-n$(SAMPLEPROVIDERNS)" CWS_FilesAndDir.mof
	@ $(ECHO) +++++ Registering FilesAndDir schema Providers  ...
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-n$(INTEROPNS)" CWS_FilesAndDirR.mof
endif

# Load the sample mof through the Client interface compiler but do not
# clean out the existing version.
repositoryServer:
	@ $(ECHO) +++++ Loading CIM_Core$(CIM_SCHEMA_VER) into $(SAMPLEPROVIDERNS) namespace ...
	@ cimmof "-I$(CIM_SCHEMA_DIR)" "-n$(SAMPLEPROVIDERNS)" $(ALLOW_EXPERIMENTAL) $(CIM_SCHEMA_DIR)/CIM_Core$(CIM_SCHEMA_VER).mof
	@ $(ECHO) +++++ Loading CIM_Event$(CIM_SCHEMA_VER) into $(SAMPLEPROVIDERNS) namespace ...
	@ cimmof "-I$(CIM_SCHEMA_DIR)" "-n$(SAMPLEPROVIDERNS)" $(ALLOW_EXPERIMENTAL) $(CIM_SCHEMA_DIR)/CIM_Event$(CIM_SCHEMA_VER).mof
	@ $(ECHO) +++++ Loading CIM_Physical$(CIM_SCHEMA_VER) into $(SAMPLEPROVIDERNS) namespace ...
	@ cimmof "-I$(CIM_SCHEMA_DIR)" "-n$(SAMPLEPROVIDERNS)" $(ALLOW_EXPERIMENTAL) $(CIM_SCHEMA_DIR)/CIM_Physical$(CIM_SCHEMA_VER).mof
	@ $(ECHO) +++++ Loading CIM_System$(CIM_SCHEMA_VER) into $(SAMPLEPROVIDERNS) namespace ...
	@ cimmof "-I$(CIM_SCHEMA_DIR)" "-n$(SAMPLEPROVIDERNS)" $(ALLOW_EXPERIMENTAL) $(CIM_SCHEMA_DIR)/CIM_System$(CIM_SCHEMA_VER).mof
	@ $(ECHO) +++++ Loading SampleProvider class definitions into $(SAMPLEPROVIDERNS) namespace ...
	@ cimmof "-n$(SAMPLEPROVIDERNS)" SampleProviderSchema.mof
	@ cimmof "-n$(SAMPLEPROVIDERNS)" AssociationProvider.mof
	@ cimmof "-n$(SAMPLEPROVIDERNS)" RT_Indication.mof
	@ cimmof "-n$(CIMV2NS)" RT_Indication.mof
	@ cimmof "-n$(SAMPLEPROVIDERNS)" ClientTest.mof
	@ $(ECHO) +++++ Registering SampleProvider schema Providers  ...
	@ cimmof "-n$(INTEROPNS)" SampleProviderSchemaR.mof
ifdef PEGASUS_ENABLE_EXECQUERY
	@ cimmof "-n$(INTEROPNS)" InstanceQueryProviderR.mof
endif

ifdef PEGASUS_ENABLE_CMPI_PROVIDER_MANAGER
	@ $(ECHO) +++++ Installing the MOF file for CMPI Providers
	@ cimmof "-n$(SAMPLEPROVIDERNS)" CWS_FilesAndDir.mof
	@ $(ECHO) +++++ Registering FilesAndDir schema Providers  ...
	@ cimmof "-n$(INTEROPNS)" CWS_FilesAndDirR.mof
endif

removerepository:
	@ $(ECHO) +++++ Removing existing repository named $(SAMPLEPROVIDERNS)
	@ $(RMREPOSITORY) $(SAMPLEPROVIDERNSDIRECTORY)

unregister:
	$(MAKE) -i unregisterproviders

unregisterproviders:
	cimprovider -r -m SampleIndicationProviderModule
	cimprovider -r -m SampleInstanceProviderModule
ifdef PEGASUS_ENABLE_EXECQUERY
	cimprovider -r -m SampleInstanceQueryProviderModule
endif
	cimprovider -r -m SampleMethodProviderModule
	cimprovider -r -m SampleAssociationProviderModule
	cimprovider -r -m RT_IndicationProviderModule
	cimprovider -r -m SampleLocalizedProviderModule
	cimprovider -r -m SimpleDisplayConsumerModule
ifdef PEGASUS_ENABLE_CMPI_PROVIDER_MANAGER
	cimprovider -r -m FilesAndDirectoriesModule
endif

depend:

sub:

misc:

tests:

poststarttests:

general:

clean:
