# Makefile builds a Sample Provider Namespace

ROOT = ../..

include $(ROOT)/mak/config.mak

INTEROPNS = root/PG_InterOp

# The following variable determines the version of the Schema to
# be loaded into the Sample Provider namespace.
# Update the following variable to change the version.

CIM_SCHEMA_VER=28
CIM_SCHEMA_DIR=CIM$(CIM_SCHEMA_VER)

SAMPLEPROVIDERNS=root/SampleProvider

createrepository:
	@ $(ECHO) +++++ Creating $(SAMPLEPROVIDERNS) namespace ...
	@ $(ECHO) +++++ Loading CIM_Core$(CIM_SCHEMA_VER) into $(SAMPLEPROVIDERNS) namespace ...
	@ $(PEGASUS_BIN_DIR)/cimmof -I$(PEGASUS_MOF_DIR)/$(CIM_SCHEMA_DIR) -n$(SAMPLEPROVIDERNS) $(PEGASUS_MOF_DIR)/$(CIM_SCHEMA_DIR)/CIM_Core$(CIM_SCHEMA_VER).mof
	@ $(ECHO) +++++ Loading CIM_System$(CIM_SCHEMA_VER) into $(SAMPLEPROVIDERNS) namespace ...
	@ $(PEGASUS_BIN_DIR)/cimmof -I$(PEGASUS_MOF_DIR)/$(CIM_SCHEMA_DIR) -n$(SAMPLEPROVIDERNS) $(PEGASUS_MOF_DIR)/$(CIM_SCHEMA_DIR)/CIM_System$(CIM_SCHEMA_VER).mof
	@ $(ECHO) +++++ Loading CIM_Event$(CIM_SCHEMA_VER) into $(SAMPLEPROVIDERNS) namespace ...
	@ $(PEGASUS_BIN_DIR)/cimmof -I$(PEGASUS_MOF_DIR)/$(CIM_SCHEMA_DIR) -n$(SAMPLEPROVIDERNS) $(PEGASUS_MOF_DIR)/$(CIM_SCHEMA_DIR)/CIM_Event$(CIM_SCHEMA_VER).mof
	@ $(ECHO) +++++ Loading class definitions into $(SAMPLEPROVIDERNS) namespace ...
	@ $(PEGASUS_BIN_DIR)/cimmof -n$(SAMPLEPROVIDERNS) SampleProviderSchema.mof
	@ $(PEGASUS_BIN_DIR)/cimmof -n$(SAMPLEPROVIDERNS) CWS_FilesAndDir.mof

registerproviders:
	@ $(ECHO) +++++ Registering providers for SampleProvider namespace  ...
	@ $(PEGASUS_BIN_DIR)/cimmof -n$(INTEROPNS) InstanceProviderR.mof
	@ $(PEGASUS_BIN_DIR)/cimmof -n$(INTEROPNS) MethodProviderR.mof
	@ $(PEGASUS_BIN_DIR)/cimmof -n$(INTEROPNS) CWS_FilesAndDirR.mof

deregisterproviders:
	@ $(ECHO) +++++ Deregistering SampleInstanceProvider ...
	@ $(PEGASUS_BIN_DIR)/cimprovider -r -m SampleInstanceProviderModule
	@ $(ECHO) +++++ Deregistering SampleMethodProvider ...
	@ $(PEGASUS_BIN_DIR)/cimprovider -r -m SampleMethodProviderModule
	@ $(ECHO) +++++ Deregistering FilesandDirectoryModule ...
	@ $(PEGASUS_BIN_DIR)/cimprovider -r -m FilesAndDirectoriesModule 
