include $(PEGASUS_ROOT)/mak/config.mak
include $(PEGASUS_ROOT)/mak/configschema.mak

CIMMOFL = cimmofl

PROVIDERNS=root/benchmark
PROVIDERNSDIRECTORY=$(REPOSITORY_ROOT)/root\#benchmark

repository:
	@ $(ECHO) +++++ Removing existing repository named $(PROVIDERNS)
	@$(RMREPOSITORY) $(PROVIDERNSDIRECTORY)
	@ $(ECHO) +++++ Loading CIM_Core$(CIM_SCHEMA_VER) into $(PROVIDERNS) namespace ...
	@ $(CIMMOFL) -R$(REPOSITORY_DIR) -I$(CIM_SCHEMA_DIR) -n$(PROVIDERNS) $(ALLOW_EXPERIMENTAL) $(CIM_SCHEMA_DIR)/CIM_Core$(CIM_SCHEMA_VER).mof
	@ $(ECHO) +++++ Loading benchmarkProvider class definitions into $(PROVIDERNS) namespace ...
	@ $(CIMMOFL) -R$(REPOSITORY_DIR) -n$(PROVIDERNS) benchmarkProvider.mof
	@ $(ECHO) +++++ Registering benchmark Provider   ...
	@ $(CIMMOFL) -R$(REPOSITORY_DIR) -n$(INTEROPNS) benchmarkProviderR.mof

repositoryServer:
	@ $(ECHO) +++++ Loading CIM_Core$(CIM_SCHEMA_VER) into $(PROVIDERNS) namespace ...
	@ cimmof -I$(CIM_SCHEMA_DIR) -n$(PROVIDERNS) $(ALLOW_EXPERIMENTAL) $(CIM_SCHEMA_DIR)/CIM_Core$(CIM_SCHEMA_VER).mof
	@ $(ECHO) +++++ Loading benchmarkProvider class definitions into $(PROVIDERNS) namespace ...
	@ cimmof -n$(PROVIDERNS) benchmarkProvider.mof
	@ $(ECHO) +++++ Registering benchmark Provider   ...
	@ cimmof -n$(INTEROPNS) benchmarkProviderR.mof

removerepository:
	@ $(ECHO) +++++ Removing existing repository named $(PROVIDERNS)
	@ $(RMREPOSITORY) $(PROVIDERNSDIRECTORY)

registerproviders:
	@ $(ECHO) +++++ Registering benchmark Provider   ...
	@ cimmof -n$(INTEROPNS) benchmarkProviderR.mof

unregister:
	$(MAKE) -i unregisterproviders

unregisterproviders:
	cimprovider -r -m benchmarkProviderModule

depend:

sub:

misc:

tests:

poststarttests:

general:

clean:

install:
