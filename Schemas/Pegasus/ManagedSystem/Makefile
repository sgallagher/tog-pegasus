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

# The following defines are used to determine the version of 
# the platform specific schema to be loaded into Managed System namespace.

MANAGEDSYSTEM_NS_SUPPORTED = true

ifeq ($(OS), HPUX)
EXTENDED_MANAGEDSYSTEM_NS_SUPPORTED = true
FILE_PREFIX=HPUX
endif

ifeq ($(MANAGEDSYSTEM_NS_SUPPORTED),true)
repository:
	@ echo +++++ Removing existing repository named $(MANAGEDSYSTEMNS)
	@ $(RMREPOSITORY) $(MANAGEDSYSTEMNSDIRECTORY)
	@ echo +++++ Loading CIM_Schema$(CIM_SCHEMA_VER) into $(MANAGEDSYSTEMNS) namespace ...
	@ $(CIMMOFCLI) -R$(REPOSITORY_DIR) -I$(CIM_SCHEMA_DIR) -n$(MANAGEDSYSTEMNS) $(CIM_SCHEMA_DIR)/CIM_Schema$(CIM_SCHEMA_VER).mof
	@ echo +++++ Loading PG_ManagedSystemSchema$(PG_SCHEMA_VER) into $(MANAGEDSYSTEMNS) namespace ...
	@ $(CIMMOFCLI) -R$(REPOSITORY_DIR) -I$(PG_MOF_PATH) -n$(MANAGEDSYSTEMNS) $(PG_MOF_PATH)/PG_ManagedSystemSchema$(PG_SCHEMA_VER).mof
	@ echo +++++ Registering Providers for PG_ManagedSystemSchema$(PG_SCHEMA_VER) ...
	@ $(CIMMOFCLI) -R$(REPOSITORY_DIR) -I$(PG_MOF_PATH) -n$(INTEROPNS) $(PG_MOF_PATH)/PG_ManagedSystemSchema$(PG_SCHEMA_VER)R.mof
ifeq ($(EXTENDED_MANAGEDSYSTEM_NS_SUPPORTED),true)
	@ echo +++++ Loading platform specific class definitions in $(MANAGEDSYSTEMNS) namespace  ...
	@ $(CIMMOFCLI) -R$(REPOSITORY_DIR) -I$(PG_MOF_PATH) -n$(MANAGEDSYSTEMNS) $(PG_MOF_PATH)/$(FILE_PREFIX)_ManagedSystemSchema$(PG_SCHEMA_VER).mof
	@ echo +++++ Registering Providers for platform specific classes in $(MANAGEDSYSTEMNS) namespace ...
	@ $(CIMMOFCLI) -R$(REPOSITORY_DIR) -I$(PG_MOF_PATH) -n$(INTEROPNS) $(PG_MOF_PATH)/$(FILE_PREFIX)_ManagedSystemSchema$(PG_SCHEMA_VER)R.mof
endif

repositoryServer:
	@ echo +++++ Loading CIM_Schema$(CIM_SCHEMA_VER) into $(MANAGEDSYSTEMNS) namespace ...
	@ cimmof -I$(CIM_SCHEMA_DIR) -n$(MANAGEDSYSTEMNS) $(CIM_SCHEMA_DIR)/CIM_Schema$(CIM_SCHEMA_VER).mof
	@ echo +++++ Loading PG_ManagedSystemSchema$(PG_SCHEMA_VER) into $(MANAGEDSYSTEMNS) namespace ...
	@ cimmof  -I$(PG_MOF_PATH) -n$(MANAGEDSYSTEMNS) $(PG_MOF_PATH)/PG_ManagedSystemSchema$(PG_SCHEMA_VER).mof
	@ echo +++++ Registering Providers for PG_ManagedSystemSchema$(PG_SCHEMA_VER) ...
	@ cimmof -I$(PG_MOF_PATH) -n$(INTEROPNS) $(PG_MOF_PATH)/PG_ManagedSystemSchema$(PG_SCHEMA_VER)R.mof
ifeq ($(EXTENDED_MANAGEDSYSTEM_NS_SUPPORTED),true)
	@ echo +++++ Loading platform specific class definitions in $(MANAGEDSYSTEMNS) namespace  ...
	@ cimmof -I$(PG_MOF_PATH) -n$(MANAGEDSYSTEMNS) $(PG_MOF_PATH)/$(FILE_PREFIX)_ManagedSystemSchema$(PG_SCHEMA_VER).mof
	@ echo +++++ Registering Providers for platform specific classes in $(MANAGEDSYSTEMNS) namespace ...
	@ cimmof -I$(PG_MOF_PATH) -n$(INTEROPNS) $(PG_MOF_PATH)/$(FILE_PREFIX)_ManagedSystemSchema$(PG_SCHEMA_VER)R.mof
endif

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
