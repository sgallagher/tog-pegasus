# Makefile builds a TestProvider Namespace
# ATTN: Today this makefile is implementation dependent.
#       It assumes that the repository is a set of directories
#       that can be removed by removing the directories.
#       This must be updated to an implementation independent repository
#       remove function.
#       Note that this is, in fact, the RMREPOSITORY function
#       which we should be able to replace with an executable
#       making this probably implementation independent.

ROOT = $(PEGASUS_ROOT)
include $(ROOT)/mak/config.mak
include $(ROOT)/mak/configschema.mak

# Loads the sample MOF into a separate namespace.
# The following variable determine the version of the Schema to
# be loaded into the TestProvider namespace.
# Update the following variable to change the version.
# The following variables determine the Namespace into which the
# TestProvider classes are to be loaded.

TESTPROVIDERNS=test/TestProvider
TESTPROVIDERNSDIRECTORY=$(REPOSITORY_ROOT)/test\#TestProvider

# Clean the existing repository and load the test MOF with cimmofl.
repository:
	@ $(ECHO) +++++ Removing existing repository named $(TESTPROVIDERNS)
	@$(RMREPOSITORY) $(TESTPROVIDERNSDIRECTORY)
	@ $(ECHO) +++++ Loading Core$(CIM_SCHEMA_VER)_Qualifiers.mof into $(TESTPROVIDERNS) namespace ...
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" -n$(TESTPROVIDERNS) $(CIM_SCHEMA_DIR)/Core$(CIM_SCHEMA_VER)_Qualifiers.mof

	@ $(ECHO) +++++ Loading TestInstanceProvider class definitions into $(TESTPROVIDERNS) namespace ...
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-n$(TESTPROVIDERNS)" TestInstanceProviderSchema.mof

	@ $(ECHO) +++++ Registering TestInstanceProvider ...
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-n$(INTEROPNS)" TestInstanceProviderRegistration.mof

ifndef PEGASUS_DISABLE_PROV_USERCTXT
	@ $(ECHO) +++++ Loading UserContextTestProvider class definitions into $(TESTPROVIDERNS) namespace ...
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-n$(TESTPROVIDERNS)" UserContextTestProvider.mof

	@ $(ECHO) +++++ Registering UserContextTestProvider ...
ifndef PEGASUS_DISABLE_PROV_USERCTXT_REQUESTOR
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-n$(INTEROPNS)" UserContextRequestorR.mof
endif
ifndef PEGASUS_DISABLE_PROV_USERCTXT_PRIVILEGED
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-n$(INTEROPNS)" UserContextPrivilegedR.mof
endif
ifndef PEGASUS_DISABLE_PROV_USERCTXT_DESIGNATED
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-n$(INTEROPNS)" UserContextDesignatedR.mof
endif
ifndef PEGASUS_DISABLE_PROV_USERCTXT_CIMSERVER
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-n$(INTEROPNS)" UserContextCIMServerR.mof
endif
endif

# Load the test MOF with cimmof.  (Do not remove the existing repository.)
repositoryServer:
	@ $(ECHO) +++++ Loading Core$(CIM_SCHEMA_VER)_Qualifiers.mof into $(TESTPROVIDERNS) namespace ...
	@ cimmof -n$(TESTPROVIDERNS) $(CIM_SCHEMA_DIR)/Core$(CIM_SCHEMA_VER)_Qualifiers.mof

ifndef PEGASUS_DISABLE_PROV_USERCTXT
	@ $(ECHO) +++++ Loading UserContextTestProvider class definitions into $(TESTPROVIDERNS) namespace ...
	@ cimmof "-n$(TESTPROVIDERNS)" UserContextTestProvider.mof

	@ $(ECHO) +++++ Registering UserContextTestProvider ...
ifndef PEGASUS_DISABLE_PROV_USERCTXT_REQUESTOR
	@ cimmof "-n$(INTEROPNS)" UserContextRequestorR.mof
endif
ifndef PEGASUS_DISABLE_PROV_USERCTXT_PRIVILEGED
	@ cimmof "-n$(INTEROPNS)" UserContextPrivilegedR.mof
endif
ifndef PEGASUS_DISABLE_PROV_USERCTXT_DESIGNATED
	@ cimmof "-n$(INTEROPNS)" UserContextDesignatedR.mof
endif
ifndef PEGASUS_DISABLE_PROV_USERCTXT_CIMSERVER
	@ cimmof "-n$(INTEROPNS)" UserContextCIMServerR.mof
endif
endif

removerepository:
	@ $(ECHO) +++++ Removing existing repository named $(TESTPROVIDERNS)
	@ $(RMREPOSITORY) $(TESTPROVIDERNSDIRECTORY)

unregister:
	$(MAKE) -i unregisterproviders

unregisterproviders:
ifndef PEGASUS_DISABLE_PROV_USERCTXT
ifndef PEGASUS_DISABLE_PROV_USERCTXT_REQUESTOR
	cimprovider -r -m UserContextRequestorModule
endif
ifndef PEGASUS_DISABLE_PROV_USERCTXT_PRIVILEGED
	cimprovider -r -m UserContextPrivilegedModule
endif
ifndef PEGASUS_DISABLE_PROV_USERCTXT_DESIGNATED
	cimprovider -r -m UserContextDesignatedModule
endif
ifndef PEGASUS_DISABLE_PROV_USERCTXT_CIMSERVER
	cimprovider -r -m UserContextCIMServerModule
endif
endif

depend:

sub:

misc:

tests:

poststarttests:

general:

clean:

