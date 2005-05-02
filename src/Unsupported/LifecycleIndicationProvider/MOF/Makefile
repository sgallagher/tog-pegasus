# Makefile builds a Sample Provider Namespace
# ATTN: Today this makefile is implementation dependent.
#       It assumes that the repository is a set of directories
#       that can be removed by removing the directories.
#       This must be updated to an implementation independent repository
#       remove function.

ROOT = ../../../..
include $(ROOT)/mak/config.mak
include $(ROOT)/mak/configschema.mak

# Loads the sample MOF into a separate namespace.
# The following variable determine the version of the Schema to
# be loaded into the Sample Provider namespace.
# Update the following variable to change the version.

SAMPLEPROVIDERNS=root/SampleProvider
SAMPLEPROVIDERNSDIRECTORY=$(REPOSITORY_ROOT)/root\#SampleProvider

# The RT_Indication class definition must also be loaded into another namespace
# for testing
CIMV2NS=root/cimv2

# Load the sample MOF with the local compiler.
repository:
	@ $(ECHO) +++++ Loading TestLifecycleIndication class definitions into $(SAMPLEPROVIDERNS) namespace...
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-n$(SAMPLEPROVIDERNS)" LifecycleIndicationProvider.mof
	@ $(ECHO) +++++ Registering Lifecycle Indication Provider...
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-n$(INTEROPNS)" LifecycleIndicationProviderR.mof

# Load the sample mof through the Client interface compiler but do not
# clean out the existing version.
repositoryServer:
	@ $(ECHO) +++++ Loading TestLifecycleIndication class definitions into $(SAMPLEPROVIDERNS) namespace...
	@ cimmof "-n$(SAMPLEPROVIDERNS)" LifecycleIndicationProvider.mof
	@ $(ECHO) +++++ Registering Lifecycle Indication Provider...
	@ cimmof "-n$(INTEROPNS)" LifecycleIndicationProviderR.mof

unregister:
	$(MAKE) -i unregisterproviders

unregisterproviders:
	cimprovider -r -m SampleLifecycleIndicationProviderModule

depend:

sub:

misc:

tests:

poststarttests:

general:

clean:
