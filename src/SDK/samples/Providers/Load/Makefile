# Makefile builds a Sample Provider Namespace

ROOT = ../..

include $(ROOT)/mak/config.mak

INTEROPNS = root/PG_InterOp

# The following variable determines the version of the Schema to
# be loaded into the Sample Provider namespace.
# Update the following variable to change the version.

CIM_SCHEMA_VER=27
CIM_SCHEMA_DIR=CIM$(CIM_SCHEMA_VER)

SAMPLEPROVIDERNS=root/SampleProvider

createrepository:
	@ $(ECHO) +++++ Creating $(SAMPLEPROVIDERNS) namespace ...
	@ $(ECHO) +++++ Loading CIM_Core$(CIM_SCHEMA_VER) into $(SAMPLEPROVIDERNS) namespace ...
	@ cimmof -I$(CIM_MOF_PATH) -n$(SAMPLEPROVIDERNS) $(CIM_MOF_PATH)/CIM_Core$(CIM_SCHEMA_VER).mof
	@ $(ECHO) +++++ Loading CIM_System$(CIM_SCHEMA_VER) into $(SAMPLEPROVIDERNS) namespace ...
	@ cimmof -I$(CIM_MOF_PATH) -n$(SAMPLEPROVIDERNS) $(CIM_MOF_PATH)/CIM_System$(CIM_SCHEMA_VER).mof
	@ $(ECHO) +++++ Loading CIM_Event$(CIM_SCHEMA_VER) into $(SAMPLEPROVIDERNS) namespace ...
	@ cimmof -I$(CIM_MOF_PATH) -n$(SAMPLEPROVIDERNS) $(CIM_MOF_PATH)/CIM_Event$(CIM_SCHEMA_VER).mof
	@ $(ECHO) +++++ Loading class definitions into $(SAMPLEPROVIDERNS) namespace ...
	@ cimmof -n$(SAMPLEPROVIDERNS) SampleProviderSchema.mof
	@ cimmof -n$(SAMPLEPROVIDERNS) RT_Indication.mof
	@ cimmof -n$(SAMPLEPROVIDERNS) CWS_FilesAndDir.mof

registerproviders:
	@ $(ECHO) +++++ Registering providers for SampleProvider namespace  ...
	@ cimmof -n$(INTEROPNS) InstanceProviderR.mof
	@ cimmof -n$(INTEROPNS) MethodProviderR.mof
	@ cimmof -n$(INTEROPNS) IndicationProviderR.mof
	@ cimmof -n$(INTEROPNS) SimpleDisplayConsumerR.mof
	@ cimmof -n$(INTEROPNS) CWS_FilesAndDirR.mof

deregisterproviders:
	@ $(ECHO) +++++ Deregistering SampleInstanceProvider ...
	@ cimprovider -r -m SampleInstanceProviderModule
	@ $(ECHO) +++++ Deregistering SampleMethodProvider ...
	@ cimprovider -r -m SampleMethodProviderModule
	@ $(ECHO) +++++ Deregistering SampleIndicationProvider ...
	@ cimprovider -r -m SampleIndicationProviderModule
	@ $(ECHO) +++++ Deregistering SampleIndicationConsumer ...
	@ cimprovider -r -m SimpleDisplayConsumerModule
	@ $(ECHO) +++++ Deregistering FilesandDirectoryModule ...
	@ cimprovider -r -m FilesAndDirectoriesModule 
