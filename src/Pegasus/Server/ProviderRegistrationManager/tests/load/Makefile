# Makefile builds a test namespace in the local directory.
# ATTN: Today this makefile is implementation dependent.
#       It assumes that the repository is a set of directories
#       that can be removed by removing the directories.
#       This must be updated to an implementation independent repository
#       remove function.

ROOT = $(PEGASUS_ROOT)
include $(ROOT)/mak/config.mak

REPOSITORYDIR = ..
NAMESPACENAME = root/PG_InterOp
NAMESPACEDIR = $(REPOSITORYDIR)/repository/root\#PG_InterOp

# Update the following two environment variables to change the version
# of the CIM_Schema to be loaded in the namespace.

CIM_SCHEMA_VER=25
CIM_SCHEMA_DIR=CIM$(CIM_SCHEMA_VER)
CIM_MOF_PATH = $(ROOT)/Schemas/$(CIM_SCHEMA_DIR)

# Update the following two environment variables to change the version
# of the PG_Schema to be loaded in the namespace.

PG_SCHEMA_VER=20
PG_SCHEMA_DIR=VER$(PG_SCHEMA_VER)
PG_MOF_PATH = $(ROOT)/Schemas/Pegasus/InterOp/$(PG_SCHEMA_DIR)

all:
	

# Delete and rebuild the repository using the MOF compiler.

localrep:
	@ echo +++++ Removing existing repository named $(NAMESPACEDIR)
	@ $(RMREPOSITORY) $(NAMESPACEDIR)
	@ echo +++++ Loading CIM_Core$(CIM_SCHEMA_VER) into $(NAMESPACEDIR) namespace ...
	@ cimmofl -I$(CIM_MOF_PATH) -R$(REPOSITORYDIR) -n$(NAMESPACENAME) $(CIM_MOF_PATH)/CIM_Core$(CIM_SCHEMA_VER).mof
	@ echo +++++ Loading PG_ProviderModule$(PG_SCHEMA_VER) into $(NAMESPACEDIR) namespace ...
	@ cimmofl -I$(PG_MOF_PATH) -R$(REPOSITORYDIR) -n$(NAMESPACENAME) $(PG_MOF_PATH)/PG_ProviderModule$(PG_SCHEMA_VER).mof

clean:
	@ echo +++++ Removing existing CIM Repository named $(NAMESPACEDIR)
	@ $(RMREPOSITORY) $(NAMESPACEDIR)

depend:

sub:

misc:

tests:

poststarttests:

general:
