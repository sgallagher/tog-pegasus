# Makefile builds a Test Provider Namepspace
# ATTN: Today this makefile is implementation dependent.
#       It assumes that the repository is a set of directories
#       that can be removed by removing the directories.
#       This must be updated to an implementation independent repository
#       remove function.

ROOT = $(PEGASUS_ROOT)
include $(ROOT)/mak/config.mak
include $(ROOT)/mak/configschema.mak

# The following variable determine the version of the Schema to
# be loaded into the Test Provider namespace.
# Update the following variable to change the version.

CIM_SCHEMA_VER=25
CIM_SCHEMA_DIR=CIM$(CIM_SCHEMA_VER)
CIM_MOF_PATH = $(ROOT)/Schemas/$(CIM_SCHEMA_DIR)

TESTPROVIDERNS=root/TestProvider
TESTPROVIDERNSDIRECTORY=$(REPOSITORY_ROOT)/root\#TestProvider

repository:
	@ $(ECHO) +++++ Removing existing repository named $(TESTPROVIDERNS)
	@$(RMREPOSITORY) $(TESTPROVIDERNSDIRECTORY)
	@ $(ECHO) +++++ Removing existing repository named $(TESTPROVIDERNS)
	@$(RMREPOSITORY) $(TESTPROVIDERNSDIRECTORY)
	@ $(ECHO) +++++ Loading CIM_Core$(CIM_SCHEMA_VER) into $(TESTPROVIDERNS) namespace ...
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-I$(CIM_MOF_PATH)" "-n$(TESTPROVIDERNS)" $(CIM_MOF_PATH)/CIM_Core$(CIM_SCHEMA_VER).mof
	@ $(ECHO) +++++ Loading TestProvider class definitions into $(TESTPROVIDERNS) namespace ...
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-n$(TESTPROVIDERNS)" ClientProviderSchema.mof
	@ $(ECHO) +++++ Registering TestProvider schema Providers  ...
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-n$(INTEROPNS)" ClientProviderSchemaR.mof

repositoryServer:
	@ $(ECHO) +++++ Loading CIM_Core$(CIM_SCHEMA_VER) into $(TESTPROVIDERNS) namespace ...
	@ cimmof "-I$(CIM_MOF_PATH)" "-n$(TESTPROVIDERNS)" $(CIM_MOF_PATH)/CIM_Core$(CIM_SCHEMA_VER).mof
	@ cimmof "-n$(TESTPROVIDERNS)" ClientProviderSchema.mof
	@ cimmof "-n$(INTEROPNS)" ClientProviderSchemaR.mof

removerepository:
	@ $(ECHO) +++++ Removing existing repository named $(TESTPROVIDERNS)
	@ $(RMREPOSITORY) $(TESTPROVIDERNSDIRECTORY)

unregister:
	$(MAKE) -i unregisterproviders

unregisterproviders:
	cimprovider -r -m TestClientProviderModule

depend:

sub:

misc:

tests:

poststarttests:

general:

clean:
