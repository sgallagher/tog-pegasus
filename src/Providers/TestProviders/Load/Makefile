# Makefile builds a test Provider Namepspace
# ATTN: Today this makefile is implementation dependent.
#       It assumes that the repository is a set of directories
#       that can be removed by removing the directories.
#       This must be updated to an implementation independent repository
#       remove function.
#       Note that this is, in fact, the REMREPOSITORY function
#       which we should be able to replace with an executable
#       making this probably implementation independent.

ROOT = $(PEGASUS_ROOT)
include $(ROOT)/mak/config.mak
include $(ROOT)/mak/configschema.mak

# Loads the sample MOF into a separate namespace.
# The following variable determine the version of the Schema to
# be loaded into the Sample Provider namespace.
# Update the following variable to change the version.
# The following variables determine the Namespace into which the
# sample provider test classes and the sample provider classes
# are to be loaded.

TESTPROVIDERNS=root/testassoc
TESTPROVIDERNSDIRECTORY=$(REPOSITORY_ROOT)/root\#testassoc

# Load the sample MOF with the local compiler. Cleans the existing repository
# before reloading.
repository:
	@ echo +++++ Removing existing repository named $(TESTPROVIDERNS)
	@$(RMREPOSITORY) $(TESTPROVIDERNSDIRECTORY)
	@ cimmofl -R$(REPOSITORY_DIR) -I$(CIM_SCHEMA_DIR) -n$(TESTPROVIDERNS) $(CIM_SCHEMA_DIR)/CIM_Core$(CIM_SCHEMA_VER).mof
	@ echo +++++ Loading CIM_Events$(CIM_SCHEMA_VER) into $(TESTPROVIDERNS) namespace ...
	@ cimmofl -R$(REPOSITORY_DIR) -I$(CIM_SCHEMA_DIR) -n$(TESTPROVIDERNS) $(CIM_SCHEMA_DIR)/CIM_Event$(CIM_SCHEMA_VER).mof
	@ echo +++++ Loading SampleProvider class definitions into $(TESTPROVIDERNS) namespace ...
	@ echo +++++ Loading AssociationTestProvider class definitions into $(TESTPROVIDERNS) namespace ...
	@ cimmofl -R$(REPOSITORY_DIR) -n$(TESTPROVIDERNS) AssociationTestProviderSchema.mof
	
	#@ echo +++++ Registering AssociationTestProvider schema Providers  ...
	#@ cimmofl -R$(REPOSITORY_DIR) -n$(INTEROPNS) AssociationTestProviderSchemaR.mof


# Load the sample mofe through the Client interface compiler but do not
# clean out the existing version.
repositoryServer:
	@ echo +++++ Loading CIM_Core$(CIM_SCHEMA_VER) into $(TESTPROVIDERNS) namespace ...
	@ cimmof -I$(CIM_MOF_PATH) -n$(TESTPROVIDERNS) $(CIM_SCHEMA_VER)/CIM_Core$(CIM_SCHEMA_VER).mof
	@ echo +++++ Loading CIM_Events$(CIM_SCHEMA_VER) into $(TESTPROVIDERNS) namespace ...
	@ cimmof -I$(CIM_MOF_PATH) -n$(TESTPROVIDERNS) $(CIM_SCHEMA_VER)/CIM_Events$(CIM_SCHEMA_VER).mof
	@ echo +++++ Loading AssociationTestProvider class definitions into $(TESTPROVIDERNS) namespace ...
	
	#@ echo +++++ Registering AssociationTestProvider schema Providers  ...
	#@ cimmofl -R$(REPOSITORY_DIR) -n$(INTEROPNS) AssociationTestProviderSchemaR.mof

removerepository:
	@ echo +++++ Removing existing repository named $(TESTPROVIDERNS)
	@ $(RMREPOSITORY) $(TESTPROVIDERNSDIRECTORY)

unregister:
	$(MAKE) -i unregisterproviders

unregisterproviders:
	cimprovider -r -m AssociationTestProviderModule

depend:

sub:

misc:

tests:

poststarttests:

general:

clean:

clitests:
		CLI rn TST_ClassA -n $(TESTPROVIDERNS)
		CLI rn TST_ClassA -rc TST_Assoc1 -n $(TESTPROVIDERNS)
		CLI rn TST_ClassA -rc TST_Assoc1 -rr from -n $(TESTPROVIDERNS)
		CLI rn TST_ClassA -rc TST_Assoc1 -rr to -n $(TESTPROVIDERNS)

		CLI rn TST_ClassB -n $(TESTPROVIDERNS)
		CLI rn TST_ClassB -rc TST_Assoc5 -n $(TESTPROVIDERNS)
		CLI rn TST_ClassB -rc TST_Assoc5 -rr from -n $(TESTPROVIDERNS)
		CLI rn TST_ClassB -rc TST_Assoc5 -rr to -n $(TESTPROVIDERNS)

		CLI rn TST_ClassC -n $(TESTPROVIDERNS)
		CLI rn TST_ClassC -rc TST_Assoc2 -n $(TESTPROVIDERNS)
		CLI rn TST_ClassC -rc TST_Assoc2 -rr from -n $(TESTPROVIDERNS)
		CLI rn TST_ClassC -rc TST_Assoc2 -rr to -n $(TESTPROVIDERNS)
		CLI rn TST_ClassC -rc TST_Assoc1 -n $(TESTPROVIDERNS)
		
		CLI rn TST_ClassD -n $(TESTPROVIDERNS)
		CLI rn TST_ClassD -rc TST_Assoc1 -n $(TESTPROVIDERNS)
		CLI rn TST_ClassD -rc TST_Assoc1 -rr from -n $(TESTPROVIDERNS)
		CLI rn TST_ClassD -rc TST_Assoc1 -rr to -n $(TESTPROVIDERNS)
