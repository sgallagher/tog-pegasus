# Makefile builds a Managed System Namepspace 
# ATTN: Today this makefile is implementation dependent.
#       It assumes that the repository is a set of directories
#       that can be removed by removing the directories.
#       This must be updated to an implementation independent repository
#       remove function.

ROOT = $(PEGASUS_ROOT)
include $(ROOT)/mak/config.mak
include $(ROOT)/mak/configschema.mak

# The following two variables determine the version of the Schema to
# be loaded into the Managed System namespace.
# Update the following two environment variables to change the version.

PG_SCHEMA_VER=20
PG_SCHEMA_DIR=VER$(PG_SCHEMA_VER)
PG_MOF_PATH=$(ROOT)/Schemas/Pegasus/ManagedSystem/$(PG_SCHEMA_DIR)

CIM_SCHEMA_VER=25
CIM_SCHEMA_DIR=CIM$(CIM_SCHEMA_VER)
CIM_MOF_PATH = $(ROOT)/Schemas/$(CIM_SCHEMA_DIR)

# The following defines are used to determine the version of 
# the platform specific schema to be loaded into Managed System namespace.

MANAGEDSYSTEM_NS_SUPPORTED = true

ifeq ($(PEGASUS_PLATFORM), HPUX_PARISC_ACC)
MS_EXTSCHEMA_VER =
MS_EXTSCHEMA_DIR = HPUX$(CIM_SCHEMA_VER)
MS_EXTPATH = $(ROOT)/Schemas/Pegasus/ManagedSystem/$MS_SCHEMA_DIR)
LOADEXTSCHEMA = 
endif

ifeq ($(MANAGEDSYSTEM_NS_SUPPORTED),true)
repository:
	@ echo +++++ Removing existing repository named $(MANAGEDSYSTEMNS)
	@ $(RMREPOSITORY) $(MANAGEDSYSTEMNSDIRECTORY)
	@ echo +++++ Loading CIM_Schema$(CIM_SCHEMA_VER) into $(MANAGEDSYSTEMNS) namespace ...
	@ $(CIMMOFCLI) -I$(CIM_MOF_PATH) -n$(MANAGEDSYSTEMNS) $(CIM_MOF_PATH)/CIM_Schema$(CIM_SCHEMA_VER).mof
	@ echo +++++ Loading PG_ManagedSystemSchema$(PG_SCHEMA_VER) into $(MANAGEDSYSTEMNS) namespace ...
	@ $(CIMMOFCLI) -I$(PG_MOF_PATH) -n$(MANAGEDSYSTEMNS) $(PG_MOF_PATH)/PG_ManagedSystemSchema$(PG_SCHEMA_VER).mof
	@ echo +++++ Registering Providers for PG_ManagedSystemSchema$(PG_SCHEMA_VER) ...
	@ $(CIMMOFCLI) -I$(PG_MOF_PATH) -n$(INTEROPNS) $(PG_MOF_PATH)/PG_ManagedSystemSchema$(PG_SCHEMA_VER)R.mof
	$(LOADEXTSCHEMA)

repositoryServer:
	@ echo +++++ Loading CIM_Schema$(CIM_SCHEMA_VER) into $(MANAGEDSYSTEMNS) namespace ...
	@ cimmof -I$(CIM_MOF_PATH) -n$(MANAGEDSYSTEMNS) $(CIM_MOF_PATH)/CIM_Schema$(CIM_SCHEMA_VER).mof
	@ echo +++++ Loading PG_ManagedSystemSchema$(PG_SCHEMA_VER) into $(MANAGEDSYSTEMNS) namespace ...
	@ cimmof  -I$(PG_MOF_PATH) -n$(MANAGEDSYSTEMNS) $(PG_MOF_PATH)/PG_ManagedSystemSchema$(PG_SCHEMA_VER).mof
	@ echo +++++ Registering Providers for PG_ManagedSystemSchema$(PG_SCHEMA_VER) ...
	@ cimmof -I$(PG_MOF_PATH) -n$(INTEROPNS) $(PG_MOF_PATH)/PG_ManagedSystemSchema$(PG_SCHEMA_VER)R.mof
	$(LOADEXTSCHEMA)

clean:
	@ echo +++++ Removing existing Repository named $(MANAGEDSYSTEMNS)
	@ $(RMREPOSITORY) $(MANAGEDSYSTEMNSDIRECTORY)
else
repository:
	@ echo +++++ No Managed System Namespace defined for this platform ++++

clean:

endif


depend:

sub:

misc:

tests:

poststarttests:

general:
