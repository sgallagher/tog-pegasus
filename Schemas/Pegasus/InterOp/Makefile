# Makefile builds the Pegasus InterOp Namepspace
# ATTN: Today this makefile is implementation dependent.
#       It assumes that the repository is a set of directories
#       that can be removed by removing the directories.
#       This must be updated to an implementation independent repository
#       remove function.

ROOT = $(PEGASUS_ROOT)
include $(ROOT)/mak/config.mak
include $(ROOT)/mak/configschema.mak

# The following two variables determine the version of the Schema to
# be loaded into the Pegasus InterOp Namespace.
# Update the following two environment variables to change the version.

PG_SCHEMA_VER=20
PG_SCHEMA_DIR=VER$(PG_SCHEMA_VER)
PG_MOF_PATH=$(ROOT)/Schemas/Pegasus/InterOp/$(PG_SCHEMA_DIR)

all:

# Delete and rebuild the repository using the MOF compiler.

repository:
	@ echo +++++ Removing existing repository named $(INTEROPNS)
	@ $(RMREPOSITORY) $(INTEROPNSDIRECTORY)
	@ echo +++++ Loading CIM_Core$(CIM_SCHEMA_VER) into $(INTEROPNS) namespace ...
	@ $(CIMMOFCLI) -R$(REPOSITORY_DIR) -I$(CIM_SCHEMA_DIR) -n$(INTEROPNS) $(ALLOW_EXPERIMENTAL) $(CIM_SCHEMA_DIR)/CIM_Core$(CIM_SCHEMA_VER).mof
	@ echo +++++ Loading CIM_Event$(CIM_SCHEMA_VER) into $(INTEROPNS) namespace ...
	@ $(CIMMOFCLI) -R$(REPOSITORY_DIR) -I$(CIM_SCHEMA_DIR) -n$(INTEROPNS) $(ALLOW_EXPERIMENTAL) $(CIM_SCHEMA_DIR)/CIM_Event$(CIM_SCHEMA_VER).mof
ifeq ($(PEGASUS_ENABLE_SLP),true) 
	@ echo +++++ Loading CIM_Interop$(CIM_SCHEMA_VER) into $(INTEROPNS) namespace ...
	@ $(CIMMOFCLI) -R$(REPOSITORY_DIR) -I$(CIM_SCHEMA_DIR) -n$(INTEROPNS) $(ALLOW_EXPERIMENTAL) $(CIM_SCHEMA_DIR)/CIM_Interop$(CIM_SCHEMA_VER).mof
endif
	@ echo +++++ Loading PG_InterOpSchema$(PG_SCHEMA_VER).mof into $(INTEROPNS) namespace...
	@ $(CIMMOFCLI) -R$(REPOSITORY_DIR) -I$(PG_MOF_PATH) -n$(INTEROPNS) $(PG_MOF_PATH)/PG_InterOpSchema$(PG_SCHEMA_VER).mof
# Bug 1222
# The following code is temporary.  These classes should be incorporated into PG_InteropSchema
# however, this cannot be done now because we cannot conditionally compile MOF.	
ifeq ($(PEGASUS_ENABLE_SLP),true) 
	@ echo +++++ Loading PG_CIMXMLCommunicationMechanism$(PG_SCHEMA_VER) into $(INTEROPNS) namespace ...
	@ $(CIMMOFCLI) -R$(REPOSITORY_DIR) -I$(PG_MOF_PATH) -n$(INTEROPNS) $(ALLOW_EXPERIMENTAL) $(PG_MOF_PATH)/PG_CIMXMLCommunicationMechanism$(PG_SCHEMA_VER).mof
	@ echo +++++ Loading PG_Namespace$(PG_SCHEMA_VER) into $(INTEROPNS) namespace ...
	@ $(CIMMOFCLI) -R$(REPOSITORY_DIR) -I$(PG_MOF_PATH) -n$(INTEROPNS) $(ALLOW_EXPERIMENTAL) $(PG_MOF_PATH)/PG_Namespace$(PG_SCHEMA_VER).mof
endif

	
install:
	$(MKDIRHIER) $(LOCAL_STATE_PREFIX)
	$(MAKE) REPOSITORY_DIR=$(LOCAL_STATE_PREFIX) repository


repositoryServer:
	@ echo +++++ Loading CIM_Core$(CIM_SCHEMA_VER) into $(INTEROPNS) namespace ...
	@ cimmof -I$(CIM_SCHEMA_DIR) -n$(INTEROPNS) $(ALLOW_EXPERIMENTAL) $(CIM_SCHEMA_DIR)/CIM_Core$(CIM_SCHEMA_VER).mof
	@ echo +++++ Loading CIM_Event$(CIM_SCHEMA_VER) into $(INTEROPNS) namespace ...
	@ cimmof -I$(CIM_SCHEMA_DIR) -n$(INTEROPNS) $(ALLOW_EXPERIMENTAL) $(CIM_SCHEMA_DIR)/CIM_Event$(CIM_SCHEMA_VER).mof
ifeq ($(PEGASUS_ENABLE_SLP),true) 
	@ echo +++++ Loading CIM_Interop$(CIM_SCHEMA_VER) into $(INTEROPNS) namespace ...
	@ cimmof -I$(CIM_SCHEMA_DIR) -n$(INTEROPNS) $(ALLOW_EXPERIMENTAL) $(CIM_SCHEMA_DIR)/CIM_Interop$(CIM_SCHEMA_VER).mof
endif
	@ echo +++++ Loading PG_InterOpSchema$(PG_SCHEMA_VER).mof into $(INTEROPNS) namespace...
	@ cimmof -I$(PG_MOF_PATH) -n$(INTEROPNS) $(PG_MOF_PATH)/PG_InterOpSchema$(PG_SCHEMA_VER).mof
# Bug 1222
# The following code is temporary.  Eventually this class should be incorporated into PG_InteropSchema
# however, this cannot be done now because we cannot conditionally compile MOF.	
ifeq ($(PEGASUS_ENABLE_SLP),true) 
	@ echo +++++ Loading Loading PG_CIMXMLCommunicationMechanism$(PG_SCHEMA_VER) into $(INTEROPNS) namespace ...
	@ cimmof -I$(PG_MOF_PATH) -n$(INTEROPNS) $(ALLOW_EXPERIMENTAL) $(PG_MOF_PATH)/Loading PG_CIMXMLCommunicationMechanism$(PG_SCHEMA_VER).mof
	@ echo +++++ Loading Loading PG_Namespace$(PG_SCHEMA_VER) into $(INTEROPNS) namespace ...
	@ cimmof -I$(PG_MOF_PATH) -n$(INTEROPNS) $(ALLOW_EXPERIMENTAL) $(PG_MOF_PATH)/Loading PG_Namespace$(PG_SCHEMA_VER).mof
endif

clean:
	@ echo +++++ Removing existing CIM Repository named $(INTEROPNS)
	@ $(RMREPOSITORY) $(INTEROPNSDIRECTORY)


depend:

sub:

misc:

tests:

poststarttests:

general:
