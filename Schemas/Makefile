# Makefile builds the Pegasus Repository
# ATTN: Today this makefile is implementation dependent.
#       It assumes that the repository is a set of directories
#       that can be removed by removing the directories.
#       This must be updated to an implementation independent repository
#       remove function.

ROOT = $(PEGASUS_ROOT)
include $(ROOT)/mak/config.mak

# Define Namespace in which the CIM Schemas are to be Loaded.
# Note that by default, the CORE is also loaded into root.
NAMESPACE=root/cimv2

all:

# Delete and rebuild the repository using the MOF compiler.

repository:
	@ echo +++++ Removing existing repository named $(REPOSITORY_ROOT)
	@ $(RMREPOSITORY) $(REPOSITORY_ROOT)
	@ echo +++++ Populating CIM repository - this will take a while
	@ echo +++++ Loading CIM_Core$(CIM_SCHEMA_VER).mof into root namespace...
	@ cimmofl -R$(REPOSITORY_DIR) -I$(CIM_SCHEMA_DIR) -nroot $(ALLOW_EXPERIMENTAL) $(CIM_SCHEMA_DIR)/CIM_Core$(CIM_SCHEMA_VER).mof
	@ echo +++++ Loading CIM_Schema$(CIM_SCHEMA_VER).mof into $(NAMESPACE) namespace...
	@ cimmofl -R$(REPOSITORY_DIR) -I$(CIM_SCHEMA_DIR) -n$(NAMESPACE) $(ALLOW_EXPERIMENTAL) $(CIM_SCHEMA_DIR)/CIM_Schema$(CIM_SCHEMA_VER).mof

# The following is a hangover from previous testing and simply loads the
# core into the namespace defined by NAMESPACE.  It should probably be 
# deleted since there is no use for it any more.
short:
	@ echo +++++ Populating CIM repository - this will take a minute
	$(RMREPOSITORY) $(REPOSITORY_ROOT)
	echo +++++ Loading CIM_Core.mof into root namespace...
	cimmofl -R$(REPOSITORY_DIR) -I$(CIM_SCHEMA_DIR) -n$(NAMESPACE) $(ALLOW_EXPERIMENTAL) $(CIM_SCHEMA_DIR)/CIM_Core$(CIM_SCHEMA_VER).mof

clean:
	@ echo +++++ Removing existing CIM Repository named $(REPOSITORY_ROOT)
	@ $(RMREPOSITORY) $(REPOSITORY_ROOT)


depend:

sub:

misc:

tests:

poststarttests:

general:

# used to generate xml output from the schema

xml:
	@ $(RMREPOSITORY) $(TMP_DIR)/repository
	@ echo +++++ Temporarily loading CIM_Core$(CIM_SCHEMA_VER) into xml/cimv2 namespace ...
	@ cimmofl -R$(TMP_DIR) -I$(CIM_SCHEMA_DIR) -nxml/cimv2 $(CIM_SCHEMA_DIR)/CIM_Core$(CIM_SCHEMA_VER).mof
	@ echo +++++ Creating XML for CIM Schema$(CIM_SCHEMA_VER).mof in file $(TMP_DIR)/CIM_Schema$(CIM_SCHEMA_VER).xml
	@ cimmofl  -R$(TMP_DIR) --xml -I$(CIM_SCHEMA_DIR) -nxml/cimv2 $(CIM_SCHEMA_DIR)/CIM_Schema$(CIM_SCHEMA_VER).mof > $(TMP_DIR)/CIM_Schema$(CIM_SCHEMA_VER).xml
	@ $(RMREPOSITORY) $(TMP_DIR)/repository
