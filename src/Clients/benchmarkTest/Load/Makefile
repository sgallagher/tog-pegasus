include $(PEGASUS_ROOT)/mak/config.mak
include $(PEGASUS_ROOT)/mak/configschema.mak

PROVIDERNS=root/benchmark
PROVIDERNSDIRECTORY=$(REPOSITORY_ROOT)/root\#benchmark

repository:
	@ echo +++++ Removing existing repository named $(PROVIDERNS)
	@$(RMREPOSITORY) $(PROVIDERNSDIRECTORY)
	@ echo +++++ Loading CIM_Core$(CIM_SCHEMA_VER) into $(PROVIDERNS) namespace ...
	@ cimmofl -R$(REPOSITORY_DIR) -I$(CIM_SCHEMA_DIR) -n$(PROVIDERNS) $(CIM_SCHEMA_DIR)/CIM_Core$(CIM_SCHEMA_VER).mof
	@ echo +++++ Loading benchmarkProvider class definitions into $(PROVIDERNS) namespace ...
	@ cimmofl -R$(REPOSITORY_DIR) -n$(PROVIDERNS) benchmarkProvider.mof
	@ echo +++++ Registering benchmark Provider   ...
	@ cimmofl -R$(REPOSITORY_DIR) -n$(INTEROPNS) benchmarkProviderR.mof

repositoryServer:
	@ echo +++++ Loading CIM_Core$(CIM_SCHEMA_VER) into $(PROVIDERNS) namespace ...
	@ cimmof -I$(CIM_SCHEMA_DIR) -n$(PROVIDERNS) $(CIM_SCHEMA_DIR)/CIM_Core$(CIM_SCHEMA_VER).mof
	@ echo +++++ Loading benchmarkProvider class definitions into $(PROVIDERNS) namespace ...
	@ cimmof -n$(PROVIDERNS) benchmarkProvider.mof
	@ echo +++++ Registering benchmark Provider   ...
	@ cimmof -n$(INTEROPNS) benchmarkProviderR.mof

removerepository:
	@ echo +++++ Removing existing repository named $(PROVIDERNS)
	@ $(RMREPOSITORY) $(PROVIDERNSDIRECTORY)

registerproviders:
	@ echo +++++ Registering benchmark Provider   ...
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

