# Makefile builds a test cimv2 namespace
# ATTN: Today this makefile is implementation dependent.
#       It assumes that the repository is a set of directories
#       that can be removed by removing the directories.
#       This must be updated to an implementation independent repository
#       remove function.

ROOT = $(PEGASUS_ROOT)
include $(ROOT)/mak/config.mak
include $(ROOT)/mak/configschema.mak

# The following variable determine the version of the Schema to
# be loaded into the test cimv2 namespace.
# Update the following variable to change the version.

CIM_SCHEMA_VER=25
CIM_SCHEMA_DIR=CIM$(CIM_SCHEMA_VER)
CIM_MOF_PATH = $(ROOT)/Schemas/$(CIM_SCHEMA_DIR)

TESTCIMV2NS=test/cimv2
TESTCIMV2NSDIRECTORY=$(REPOSITORY_ROOT)/test\#cimv2

repository:
	@ echo +++++ Removing existing repository named $(TESTCIMV2NS)
	@$(RMREPOSITORY) $(TESTCIMV2NSDIRECTORY)
	@ echo +++++ Loading CIM_Schema$(CIM_SCHEMA_VER) into $(TESTCIMV2NS) namespace ...
	@ cimmofl -R$(REPOSITORY_DIR) -I$(CIM_MOF_PATH) -n$(TESTCIMV2NS) $(CIM_MOF_PATH)/CIM_Schema$(CIM_SCHEMA_VER).mof

repositoryServer:
	@ echo +++++ Loading CIM_Schema$(CIM_SCHEMA_VER) into $(TESTCIMV2NS) namespace ...
	@ cimmof -I$(CIM_MOF_PATH) -n$(TESTCIMV2NS) $(CIM_MOF_PATH)/CIM_Schema$(CIM_SCHEMA_VER).mof

removerepository:
	@ echo +++++ Removing existing repository named $(TESTCIMV2NS)
	@$(RMREPOSITORY) $(TESTCIMV2NSDIRECTORY)

clean:

depend:

sub:

misc:

tests:

poststarttests:

general:
