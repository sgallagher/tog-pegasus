# Makefile builds the Pegasus InterOp Namepspace
# ATTN: Today this makefile is implementation dependent.
#       It assumes that the repository is a set of directories
#       that can be removed by removing the directories.
#       This must be updated to an implementation independent repository
#       remove function.

ROOT = $(PEGASUS_ROOT)
include $(ROOT)/mak/config.mak

# The PG_INTEROPNS variable defines the absolute name of the 
# Pegasus InterOp Namespace.

PG_INTEROPNS=root/PG_InterOp
PG_INTEROPNSDIRECTORY = $(REPOSITORY_ROOT)/root\#PG_InterOp

# The following two variables determine the version of the Schema to
# be loaded into the Pegasus InterOp Namespace.
# Update the following two environment variables to change the version.

PG_SCHEMA_VER=20
PG_SCHEMA_DIR=VER$(PG_SCHEMA_VER)
PG_MOF_PATH=$(ROOT)/Schemas/Pegasus/InterOp/$(PG_SCHEMA_DIR)

CIM_SCHEMA_VER=25
CIM_SCHEMA_DIR=CIM$(CIM_SCHEMA_VER)
CIM_MOF_PATH = $(ROOT)/Schemas/$(CIM_SCHEMA_DIR)

all:

# Delete and rebuild the repository using the MOF compiler.

repository:
	@ echo +++++ Removing existing repository named $(PG_INTEROPNS)
	@ $(RMREPOSITORY) $(PG_INTEROPNSDIRECTORY)
	@ echo +++++ Loading CIM_Core$(CIM_SCHEMA_VER) into $(PG_INTEROPNS) namespace ...
	@ cimmofl -I$(CIM_MOF_PATH) -n$(PG_INTEROPNS) $(CIM_MOF_PATH)/CIM_Core$(CIM_SCHEMA_VER).mof
	@ echo +++++ Loading PG_Events$(PG_SCHEMA_VER).mof into $(PG_INTEROPNS) namespace...
	@ cimmofl -I$(PG_MOF_PATH) -n$(PG_INTEROPNS) $(PG_MOF_PATH)/PG_Events$(PG_SCHEMA_VER).mof
	@ echo +++++ Loading PG_ProviderModule$(PG_SCHEMA_VER).mof into $(PG_INTEROPNS) namespace...
	@ cimmofl -I$(PG_MOF_PATH) -n$(PG_INTEROPNS) $(PG_MOF_PATH)/PG_ProviderModule$(PG_SCHEMA_VER).mof


clean:
	@ echo +++++ Removing existing CIM Repository named $(REPOSITORY_ROOT)
	@ $(RMREPOSITORY) $(PG_INTEROPNSDIRECTORY)


depend:

sub:

misc:

tests:

poststarttests:

general:
