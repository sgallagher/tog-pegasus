# Makefile builds the Pegasus Internal Namepspace
# ATTN: Today this makefile is implementation dependent.
#       It assumes that the repository is a set of directories
#       that can be removed by removing the directories.
#       This must be updated to an implementation independent repository
#       remove function.

ROOT = $(PEGASUS_ROOT)
include $(ROOT)/mak/config.mak

# The PG_INTERNALNS variable defines the absolute name of the 
# Pegasus INTERNALNS Namespace.

PG_INTERNALNS = root/PG_Internal
PG_INTERNALNSDIRECTORY = $(REPOSITORY_ROOT)/root\#PG_Internal

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
	@ echo +++++ Removing existing repository named $(PG_INTERNALNS)
	@ $(RMREPOSITORY) $(PG_INTERNALNSDIRECTORY)
	@ echo +++++ Loading CIM_Qualifiers$(CIM_SCHEMA_VER) into $(PG_INTERNALNS) namespace ...
	@ cimmof  -n$(PG_INTERNALNS) ../CIM_Qualifiers$(CIM_SCHEMA_VER).mof
	@ echo +++++ Loading PG_User$(PG_SCHEMA_VER).mof into $(PG_INTERNALNS) namespace...
	@ cimmof -I$(PG_MOF_PATH) -n$(PG_INTERNALNS) $(PG_MOF_PATH)/PG_User$(PG_SCHEMA_VER).mof
	@ echo +++++ Loading PG_Authorization$(PG_SCHEMA_VER).mof into $(PG_INTERNALNS) namespace...
	@ cimmof -I$(PG_MOF_PATH) -n$(PG_INTERNALNS) $(PG_MOF_PATH)/PG_Authorization$(PG_SCHEMA_VER).mof
	@ echo +++++ Loading PG_ConfigSetting$(PG_SCHEMA_VER).mof into $(PG_INTERNALNS) namespace...
	@ cimmof -I$(PG_MOF_PATH) -n$(PG_INTERNALNS) $(PG_MOF_PATH)/PG_ConfigSetting$(PG_SCHEMA_VER).mof

clean:
	@ echo +++++ Removing existing CIM Repository named $(REPOSITORY_ROOT)
	@ $(RMREPOSITORY) $(PG_INTERNALNSDIRECTORY)


depend:

sub:

misc:

tests:

poststarttests:

general:
