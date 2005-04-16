# Makefile builds a Sample Provider Namespace

ROOT = ../..

include $(ROOT)/mak/config.mak

INTEROPNS = root/PG_InterOp

SAMPLEPROVIDERNS=root/SampleProvider

createrepository:
	@ $(ECHO) +++++ Creating $(SAMPLEPROVIDERNS) namespace ...
	@ $(ECHO) +++++ Loading CIM_Core$(CIM_SCHEMA_VER) into $(SAMPLEPROVIDERNS) namespace ...
	@ $(PEGASUS_BIN_DIR)/cimmof -I$(PEGASUS_MOF_DIR)/$(PEGASUS_CIM_SCHEMA) -n$(SAMPLEPROVIDERNS) $(PEGASUS_MOF_DIR)/$(PEGASUS_CIM_SCHEMA)/CIM_Core$(CIM_SCHEMA_VER).mof
	@ $(ECHO) +++++ Loading CIM_Physical$(CIM_SCHEMA_VER) into $(SAMPLEPROVIDERNS) namespace ...
	@ $(PEGASUS_BIN_DIR)/cimmof -I$(PEGASUS_MOF_DIR)/$(PEGASUS_CIM_SCHEMA) -n$(SAMPLEPROVIDERNS) $(PEGASUS_MOF_DIR)/$(PEGASUS_CIM_SCHEMA)/CIM_Physical$(CIM_SCHEMA_VER).mof
	@ $(ECHO) +++++ Loading CIM_System$(CIM_SCHEMA_VER) into $(SAMPLEPROVIDERNS) namespace ...
	@ $(PEGASUS_BIN_DIR)/cimmof -I$(PEGASUS_MOF_DIR)/$(PEGASUS_CIM_SCHEMA) -n$(SAMPLEPROVIDERNS) $(PEGASUS_MOF_DIR)/$(PEGASUS_CIM_SCHEMA)/CIM_System$(CIM_SCHEMA_VER).mof
	@ $(ECHO) +++++ Loading CIM_Event$(CIM_SCHEMA_VER) into $(SAMPLEPROVIDERNS) namespace ...
	@ $(PEGASUS_BIN_DIR)/cimmof -I$(PEGASUS_MOF_DIR)/$(PEGASUS_CIM_SCHEMA) -n$(SAMPLEPROVIDERNS) $(PEGASUS_MOF_DIR)/$(PEGASUS_CIM_SCHEMA)/CIM_Event$(CIM_SCHEMA_VER).mof
	@ $(ECHO) +++++ Loading class definitions into $(SAMPLEPROVIDERNS) namespace ...
	@ $(PEGASUS_BIN_DIR)/cimmof -n$(SAMPLEPROVIDERNS) SampleProviderSchema.mof

registerproviders:
	@ $(ECHO) +++++ Registering providers for SampleProvider namespace  ...
	@ $(PEGASUS_BIN_DIR)/cimmof -n$(INTEROPNS) InstanceProviderR.mof
	@ $(PEGASUS_BIN_DIR)/cimmof -n$(INTEROPNS) MethodProviderR.mof
	@ $(PEGASUS_BIN_DIR)/cimmof -n$(INTEROPNS) IndicationProviderR.mof
	@ $(PEGASUS_BIN_DIR)/cimmof -n$(INTEROPNS) SimpleDisplayConsumerR.mof

deregisterproviders:
	@ $(ECHO) +++++ Deregistering SampleInstanceProvider ...
	@ $(PEGASUS_BIN_DIR)/cimprovider -r -m SampleInstanceProviderModule
	@ $(ECHO) +++++ Deregistering SampleMethodProvider ...
	@ $(PEGASUS_BIN_DIR)/cimprovider -r -m SampleMethodProviderModule
	@ $(ECHO) +++++ Deregistering SampleIndicationProvider ...
	@ $(PEGASUS_BIN_DIR)/cimprovider -r -m SampleIndicationProviderModule
	@ $(ECHO) +++++ Deregistering SimpleDisplayConsumerModule ...
	@ $(PEGASUS_BIN_DIR)/cimprovider -r -m SimpleDisplayConsumerModule
