# Makefile builds a Sample Provider Namepspace
# ATTN: Today this makefile is implementation dependent.
#       It assumes that the repository is a set of directories
#       that can be removed by removing the directories.
#       This must be updated to an implementation independent repository
#       remove function.

ROOT = $(PEGASUS_ROOT)
include $(ROOT)/mak/config.mak
include $(ROOT)/mak/configschema.mak

# The following variable determine the version of the Schema to
# be loaded into the Sample Provider namespace.
# Update the following variable to change the version.

SAMPLEPROVIDERNS=root/SampleProvider
SAMPLEPROVIDERNSDIRECTORY=$(REPOSITORY_ROOT)/root\#SampleProvider

repository:
	@ echo +++++ Removing existing repository named $(SAMPLEPROVIDERNS)
	@$(RMREPOSITORY) $(SAMPLEPROVIDERNSDIRECTORY)
	@ echo +++++ Removing existing repository named $(SAMPLEPROVIDERNS)
	@$(RMREPOSITORY) $(SAMPLEPROVIDERNSDIRECTORY)
	@ echo +++++ Loading CIM_Core$(CIM_SCHEMA_VER) into $(SAMPLEPROVIDERNS) namespace ...
	@ cimmofl -R$(REPOSITORY_DIR) -I$(CIM_SCHEMA_DIR) -n$(SAMPLEPROVIDERNS) $(CIM_SCHEMA_DIR)/CIM_Core$(CIM_SCHEMA_VER).mof
	@ echo +++++ Loading CIM_Event$(CIM_SCHEMA_VER) into $(SAMPLEPROVIDERNS) namespace ...
	@ cimmofl -R$(REPOSITORY_DIR) -I$(CIM_SCHEMA_DIR) -n$(SAMPLEPROVIDERNS) $(CIM_SCHEMA_DIR)/CIM_Event$(CIM_SCHEMA_VER).mof
	@ echo +++++ Loading SampleProvider class definitions into $(SAMPLEPROVIDERNS) namespace ...
	@ cimmofl -R$(REPOSITORY_DIR) -n$(SAMPLEPROVIDERNS) SampleProviderSchema.mof
	@ cimmofl -R$(REPOSITORY_DIR) -n$(SAMPLEPROVIDERNS) RT_Indication.mof
	@ echo +++++ Registering SampleProvider schema Providers  ...
	@ cimmofl -R$(REPOSITORY_DIR) -n$(INTEROPNS) SampleProviderSchemaR.mof

repositoryServer:
	@ echo +++++ Loading CIM_Core$(CIM_SCHEMA_VER) into $(SAMPLEPROVIDERNS) namespace ...
	@ cimmof -I$(CIM_SCHEMA_DIR) -n$(SAMPLEPROVIDERNS) $(CIM_SCHEMA_DIR)/CIM_Core$(CIM_SCHEMA_VER).mof
	@ echo +++++ Loading CIM_Event$(CIM_SCHEMA_VER) into $(SAMPLEPROVIDERNS) namespace ...
	@ cimmof -I$(CIM_SCHEMA_DIR) -n$(SAMPLEPROVIDERNS) $(CIM_SCHEMA_DIR)/CIM_Event$(CIM_SCHEMA_VER).mof
	@ echo +++++ Loading SampleProvider class definitions into $(SAMPLEPROVIDERNS) namespace ...
	@ cimmof -n$(SAMPLEPROVIDERNS) SampleProviderSchema.mof
	@ cimmof -n$(SAMPLEPROVIDERNS) RT_Indication.mof
	@ echo +++++ Registering SampleProvider schema Providers  ...
	@ cimmof -n$(INTEROPNS) SampleProviderSchemaR.mof

removerepository:
	@ echo +++++ Removing existing repository named $(SAMPLEPROVIDERNS)
	@ $(RMREPOSITORY) $(SAMPLEPROVIDERNSDIRECTORY)

unregister:
	$(MAKE) -i unregisterproviders

unregisterproviders:
	cimprovider -r -m SampleIndicationProviderModule
	cimprovider -r -m SampleInstanceProviderModule
	cimprovider -r -m SampleMethodProviderModule
	cimprovider -r -m RT_IndicationProviderModule

depend:

sub:

misc:

tests:

poststarttests:

general:

clean:
