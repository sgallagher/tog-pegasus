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

TESTCIMV2NS=test/cimv2
TESTCIMV2NSDIRECTORY=$(REPOSITORY_ROOT)/test\#cimv2

CIM_SCHEMA_VER=25
CIM_SCHEMA_DIR=$(PEGASUS_ROOT)/Schemas/CIM25

# ***** CIM_SCHEMA_DIR INFO ****
# If CIM_SCHEMA_DIR changes to use a preliminary schema which
# has experimentals make sure and change the path below to appopriate
# directory path.  Example:  CIMPrelim271 is preliminary and has
# experimental classes.  Since experimental classes exist the -aE
# option of the mof compiler needs to be set.
# *****
ifeq ($(CIM_SCHEMA_DIR), $(PEGASUS_ROOT)/Schemas/CIMPrelim271)
ALLOW_EXPERIMENTAL = -aE
else
ALLOW_EXPERIMENTAL = -aE
endif

repository:
	@ echo +++++ Removing existing repository named $(TESTCIMV2NS)
	@$(RMREPOSITORY) $(TESTCIMV2NSDIRECTORY)
	@ echo +++++ Loading CIM_Schema$(CIM_SCHEMA_VER) into $(TESTCIMV2NS) namespace ...
	@ cimmofl -R$(REPOSITORY_DIR) -I$(CIM_SCHEMA_DIR) -n$(TESTCIMV2NS) $(ALLOW_EXPERIMENTAL) $(CIM_SCHEMA_DIR)/CIM_Schema$(CIM_SCHEMA_VER).mof

repositoryServer:
	@ echo +++++ Loading CIM_Schema$(CIM_SCHEMA_VER) into $(TESTCIMV2NS) namespace ...
	@ cimmof -I$(CIM_SCHEMA_DIR) -n$(TESTCIMV2NS) $(ALLOW_EXPERIMENTAL) $(CIM_SCHEMA_DIR)/CIM_Schema$(CIM_SCHEMA_VER).mof

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
