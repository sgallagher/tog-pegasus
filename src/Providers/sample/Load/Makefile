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

CIM_SCHEMA_VER=25
CIM_SCHEMA_DIR=CIM$(CIM_SCHEMA_VER)
CIM_MOF_PATH = $(ROOT)/Schemas/$(CIM_SCHEMA_DIR)

SAMPLEPROVIDERNS=root/SampleProvider
SAMPLEPROVIDERNSDIRECTORY=$(REPOSITORY_ROOT)/root\#SampleProvider

repository:
	@ echo +++++ Removing existing repository named $(SAMPLEPROVIDERNS)
	@$(RMREPOSITORY) $(SAMPLEPROVIDERNSDIRECTORY)
	@ echo +++++ Loading CIM_Core$(CIM_SCHEMA_VER) into $(SAMPLEPROVIDERNS) namespace ...
	@ cimmofl -I$(CIM_MOF_PATH) -n$(SAMPLEPROVIDERNS) $(CIM_MOF_PATH)/CIM_Core$(CIM_SCHEMA_VER).mof
	@ echo +++++ Loading CIM_Events$(CIM_SCHEMA_VER) into $(SAMPLEPROVIDERNS) namespace ...
	@ cimmofl -I$(CIM_MOF_PATH) -n$(SAMPLEPROVIDERNS) $(CIM_MOF_PATH)/CIM_Events$(CIM_SCHEMA_VER).mof
	@ echo +++++ Loading SampleProvider class definitions into $(SAMPLEPROVIDERNS) namespace ...
	@ cimmofl -n$(SAMPLEPROVIDERNS) SampleProviderSchema.mof
	@ echo +++++ Registering SampleProvider schema Providers  ...
	@ cimmofl -n$(INTEROPNS) SampleProviderSchemaR.mof

removerepository:
	@ echo +++++ Removing existing repository named $(SAMPLEPROVIDERNS)
	@ $(RMREPOSITORY) $(SAMPLEPROVIDERNSDIRECTORY)

depend:

sub:

misc:

tests:

poststarttests:

general:

clean:
