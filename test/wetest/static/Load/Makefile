# Makefile builds a static test Namepspace 
# ATTN: Today this makefile is implementation dependent.
#       It assumes that the repository is a set of directories
#       that can be removed by removing the directories.
#       This must be updated to an implementation independent repository
#       remove function.

ROOT = $(PEGASUS_ROOT)
include $(ROOT)/mak/config.mak
include $(ROOT)/mak/configschema.mak

# The following variable determine the version of the Schema to
# be loaded into the static tests namespace.
# Update the following variable to change the version.

CIM_SCHEMA_VER=25
CIM_SCHEMA_DIR=CIM$(CIM_SCHEMA_VER)
CIM_MOF_PATH = $(ROOT)/Schemas/$(CIM_SCHEMA_DIR)

STATICTESTNS=test/static
STATICTESTNSDIRECTORY=$(REPOSITORY_ROOT)/test\#static

repository:
	@ echo +++++ Removing existing repository named $(STATICTESTNS)
	@$(RMREPOSITORY) $(STATICTESTNSDIRECTORY)
	@ echo +++++ Loading CIM_Core$(CIM_SCHEMA_VER) into $(STATICTESTNS) namespace ...
	@ cimmofl -I$(CIM_MOF_PATH) -n$(STATICTESTNS) $(CIM_MOF_PATH)/CIM_Core$(CIM_SCHEMA_VER).mof
	@ echo +++++ Loading Static Test Schema into $(STATICTESTNS) namespace ...
	@ cimmofl -n$(STATICTESTNS) StaticTestSchema.mof
	@ echo +++++ Registering Static Tests Schema Providers  ...
	@ cimmofl -n$(INTEROPNS) StaticTestSchemaR.mof

repositoryServer:
	@ echo +++++ Loading CIM_Core$(CIM_SCHEMA_VER) into $(STATICTESTNS) namespace ...
	@ cimmof -I$(CIM_MOF_PATH) -n$(STATICTESTNS) $(CIM_MOF_PATH)/CIM_Core$(CIM_SCHEMA_VER).mof
	@ echo +++++ Loading Static Test Schema into $(STATICTESTNS) namespace ...
	@ cimmof -n$(STATICTESTNS) StaticTestSchema.mof
	@ echo +++++ Registering Static Tests Schema Providers  ...
	@ cimmof -n$(INTEROPNS) StaticTestSchemaR.mof
removerepository:
	@ echo +++++ Removing existing repository named $(STATICTESTNS)
	@$(RMREPOSITORY) $(STATICTESTNSDIRECTORY)

clean:

depend:

sub:

misc:

tests:

poststarttests:

general:
