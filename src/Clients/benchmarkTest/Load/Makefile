include $(PEGASUS_ROOT)/mak/config.mak
include $(PEGASUS_ROOT)/mak/configschema.mak
include $(PEGASUS_ROOT)/mak/test.mak
include $(PEGASUS_ROOT)/mak/commands.mak

ifdef TESTID
   BenchmarkMOF_NAME = benchmarkProvider_$(TESTID)
else
   BenchmarkMOF_NAME = benchmarkProvider
endif

PROVIDERNS=root/benchmark
PROVIDERNSDIRECTORY=$(REPOSITORY_ROOT)/root\#benchmark

CIMMOFL = cimmofl

repository:
	@ $(ECHO) +++++ Removing existing repository named $(PROVIDERNS)
	@$(RMREPOSITORY) $(PROVIDERNSDIRECTORY)
	@ $(ECHO) +++++ Loading CIM_Core$(CIM_SCHEMA_VER) into $(PROVIDERNS) namespace ...
	@ $(CIMMOFL) "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-I$(CIM_SCHEMA_DIR)" \
              "-n$(PROVIDERNS)" $(ALLOW_EXPERIMENTAL) $(CIM_SCHEMA_DIR)/CIM_Core$(CIM_SCHEMA_VER).mof
	@ $(ECHO) +++++ Loading $(BenchmarkMOF_NAME) class definitions into $(PROVIDERNS) namespace ...
	@ $(CIMMOFL) "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-n$(PROVIDERNS)" $(BenchmarkMOF_NAME).mof
	@ $(ECHO) +++++ Registering benchmark Provider   ...
	@ $(CIMMOFL) "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-n$(INTEROPNS)" $(BenchmarkMOF_NAME)R.mof

repositoryServer:
	@ $(ECHO) +++++ Loading CIM_Core$(CIM_SCHEMA_VER) into $(PROVIDERNS) namespace ...
	@ cimmof "-I$(CIM_SCHEMA_DIR)" "-n$(PROVIDERNS)" $(ALLOW_EXPERIMENTAL) $(CIM_SCHEMA_DIR)/CIM_Core$(CIM_SCHEMA_VER).mof
	@ $(ECHO) +++++ Loading $(BenchmarkMOF_NAME) class definitions into $(PROVIDERNS) namespace ...
	@ cimmof "-n$(PROVIDERNS)" $(BenchmarkMOF_NAME).mof
	@ $(ECHO) +++++ Registering benchmark Provider   ...
	@ cimmof "-n$(INTEROPNS)" $(BenchmarkMOF_NAME)R.mof

removerepository:
	@ $(ECHO) +++++ Removing existing repository named $(PROVIDERNS)
	@ $(RMREPOSITORY) $(PROVIDERNSDIRECTORY)

registerproviders:
	@ $(ECHO) +++++ Registering benchmark Provider   ...
	@ cimmof "-n$(INTEROPNS)" benchmarkProviderR.mof

unregister:
	$(MAKE) -i unregisterproviders

unregisterproviders:
	cimprovider -r -m benchmarkProviderModule

changeconfig:
	$(MAKE) -i cimstart
	$(MAKE) unregister
	$(MAKE) -i cimstop
	$(MAKE) removerepository
	$(MAKE) cimstart
ifdef TESTID
	$(MAKE) repositoryServer TESTID=$(TESTID)
else
	$(MAKE) repositoryServer
endif


depend:

sub:

misc:

tests:

poststarttests:

general:

clean:

