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

STATICTESTNS=test/static
STATICTESTNSDIRECTORY=$(REPOSITORY_ROOT)/test\#static

repository:
	@ $(ECHO) +++++ Removing existing repository named $(STATICTESTNS)
	@$(RMREPOSITORY) $(STATICTESTNSDIRECTORY)
	@ $(ECHO) +++++ Loading CIM_Core$(CIM_SCHEMA_VER) into $(STATICTESTNS) namespace ...
	@ cimmofl -R$(REPOSITORY_DIR) -I$(CIM_SCHEMA_DIR) -n$(STATICTESTNS) $(ALLOW_EXPERIMENTAL) $(CIM_SCHEMA_DIR)/CIM_Core$(CIM_SCHEMA_VER).mof
	@ $(ECHO) +++++ Loading WET_Schema into $(STATICTESTNS) namespace ...
	@ cimmofl -R$(REPOSITORY_DIR) -n$(STATICTESTNS) WET_Schema.mof
	@ $(ECHO) +++++ Loading Static Test Schema into $(STATICTESTNS) namespace ...
	@ cimmofl -R$(REPOSITORY_DIR) -n$(STATICTESTNS) StaticTestSchema.mof
	@ $(ECHO) +++++ Registering Static Tests Schema Providers  ...
	@ cimmofl -R$(REPOSITORY_DIR) -n$(INTEROPNS) StaticTestSchemaR.mof

repositoryServer:
	@ $(ECHO) +++++ Loading CIM_Core$(CIM_SCHEMA_VER) into $(STATICTESTNS) namespace ...
	@ cimmof -I$(CIM_SCHEMA_DIR) -n$(STATICTESTNS) $(ALLOW_EXPERIMENTAL) $(CIM_SCHEMA_DIR)/CIM_Core$(CIM_SCHEMA_VER).mof
	@ $(ECHO) +++++ Loading WET_Schema into $(STATICTESTNS) namespace ...
	@ cimmof -n$(STATICTESTNS) WET_Schema.mof
	@ $(ECHO) +++++ Loading Static Test Schema into $(STATICTESTNS) namespace ...
	@ cimmof -n$(STATICTESTNS) StaticTestSchema.mof
	@ $(ECHO) +++++ Registering Static Tests Schema Providers  ...
	@ cimmof -n$(INTEROPNS) StaticTestSchemaR.mof

removerepository:
	@ $(ECHO) +++++ Removing existing repository named $(STATICTESTNS)
	@$(RMREPOSITORY) $(STATICTESTNSDIRECTORY)

clean:

depend:

sub:

misc:

tests:

messages:

poststarttests:

general:
install:
