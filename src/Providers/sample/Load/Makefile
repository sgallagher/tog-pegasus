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

CIM_SCHEMA_VER=25
CIM_SCHEMA_DIR=CIM$(CIM_SCHEMA_VER)
CIM_MOF_PATH = $(ROOT)/Schemas/$(CIM_SCHEMA_DIR)

SAMPLEPROVIDERNS=root/SampleProvider
SAMPLEPROVIDERNSDIRECTORY=$(REPOSITORY_ROOT)/root\#SampleProvider

# The following defines are used to determine the version of 
# the platform specific schema to be loaded into SampleProvider namespace.

repository:
	@ echo +++++ Removing existing repository named $(SAMPLEPROVIDERNS)
	@$(RMREPOSITORY) $(SAMPLEPROVIDERNSDIRECTORY)
	@ echo +++++ Loading CIM_Core$(CIM_SCHEMA_VER) into $(SAMPLEPROVIDERNS) namespace ...
	@ cimmofl -I$(CIM_MOF_PATH) -n$(SAMPLEPROVIDERNS) $(CIM_MOF_PATH)/CIM_Core$(CIM_SCHEMA_VER).mof
	@ echo +++++ Loading SampleProvider.mof into $(SAMPLEPROVIDERNS) namespace ...
	@ cimmofl -n$(SAMPLEPROVIDERNS) SampleProvider.mof
	@ echo +++++ Loading MethodProvider.mof into $(SAMPLEPROVIDERNS) namespace ...
	@ cimmofl -n$(SAMPLEPROVIDERNS) MethodProvider.mof
	@ echo +++++ Registering MethodProvider ...
	@ cimmofl -n$(INTEROPNS) MethodProviderR.mof

clean:
	@ echo +++++ Removing existing repository named $(SAMPLEPROVIDERNS)
	@ $(RMREPOSITORY) $(SAMPLEPROVIDERNSDIRECTORY)

depend:

sub:

misc:

tests:

poststarttests:

general:
