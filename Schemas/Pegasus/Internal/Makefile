# Makefile builds the Pegasus Internal Namepspace
# ATTN: Today this makefile is implementation dependent.
#       It assumes that the repository is a set of directories
#       that can be removed by removing the directories.
#       This must be updated to an implementation independent repository
#       remove function.

ROOT = $(PEGASUS_ROOT)
include $(ROOT)/mak/config.mak
include $(ROOT)/mak/configschema.mak

# The following two variables determine the version of the Schema to
# be loaded into the Pegasus Internal Namespace.
# Update the following two environment variables to change the version.

PG_SCHEMA_VER=20
PG_SCHEMA_DIR=VER$(PG_SCHEMA_VER)
PG_MOF_PATH=$(ROOT)/Schemas/Pegasus/Internal/$(PG_SCHEMA_DIR)

CIM_SCHEMA_VER=25
CIM_SCHEMA_DIR=CIM$(CIM_SCHEMA_VER)
CIM_MOF_PATH = $(ROOT)/Schemas/$(CIM_SCHEMA_DIR)

all:

# Delete and rebuild the repository using the MOF compiler.

repository:
	@ echo +++++ Removing existing repository named $(INTERNALNS)
	@ $(RMREPOSITORY) $(INTERNALNSDIRECTORY)
	@ echo +++++ Loading CIM_Qualifiers$(CIM_SCHEMA_VER) into $(INTERNALNS) namespace ...
	@ $(CIMMOFCLI)  -n$(INTERNALNS) ../CIM_Qualifiers$(CIM_SCHEMA_VER).mof
	@ echo +++++ Loading PG_InternalSchema$(PG_SCHEMA_VER).mof into $(INTERNALNS) namespace...
	@ $(CIMMOFCLI) -I$(PG_MOF_PATH) -n$(INTERNALNS) $(PG_MOF_PATH)/PG_InternalSchema$(PG_SCHEMA_VER).mof

clean:
	@ echo +++++ Removing existing CIM Repository named $(REPOSITORY_ROOT)
	@ $(RMREPOSITORY) $(INTERNALNSDIRECTORY)


depend:

sub:

misc:

tests:

poststarttests:

general:
