ROOT=$(PEGASUS_ROOT)

DIR = Pegasus/ControlProviders/QueryCapabilitiesProvider/tests

include $(ROOT)/mak/config.mak

LIBRARIES = \
	pegclient \
   	pegcommon

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY 

PROGRAM = testCIMQueryCap
SOURCES = TestCIMQueryCap.cpp

include $(ROOT)/mak/program.mak

CIM_SCHEMA_DIR = $(ROOT)/Schemas/CIMPrelim291
ALLOW_EXPERIMENTAL = -aE
SAMPLE_PROVIDER_NS = root/SampleProvider

tests:
 
repository:
	@ echo +++++ Loading $(CIM_SCHEMA_DIR)/CIM_Interop.mof into root/cimv2 namespace...
	@ cimmofl -R$(REPOSITORY_DIR) -I$(CIM_SCHEMA_DIR) $(ALLOW_EXPERIMENTAL) $(CIM_SCHEMA_DIR)/CIM_Interop.mof
	@ echo +++++ Loading $(CIM_SCHEMA_DIR)/CIM_Interop.mof into $(SAMPLE_PROVIDER_NS) namespace...
	@ cimmofl -R$(REPOSITORY_DIR) -I$(CIM_SCHEMA_DIR) $(ALLOW_EXPERIMENTAL) -n$(SAMPLE_PROVIDER_NS) $(CIM_SCHEMA_DIR)/CIM_Interop.mof


repositoryServer:
	@ echo +++++ Loading $(CIM_SCHEMA_DIR)/CIM_Interop.mof into root/cimv2 namespace...
	@ cimmof -I$(CIM_SCHEMA_DIR) $(ALLOW_EXPERIMENTAL) $(CIM_SCHEMA_DIR)/CIM_Interop.mof
	@ echo +++++ Loading $(CIM_SCHEMA_DIR)/CIM_Interop.mof into $(SAMPLE_PROVIDER_NS) namespace...
	@ cimmof -I$(CIM_SCHEMA_DIR) $(ALLOW_EXPERIMENTAL) -n$(SAMPLE_PROVIDER_NS) $(CIM_SCHEMA_DIR)/CIM_Interop.mof


poststarttests:
	$(PROGRAM)
