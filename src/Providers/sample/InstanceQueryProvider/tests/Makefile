# Test the Family Provider using the script toosl

ROOT = $(PEGASUS_ROOT)
include $(ROOT)/mak/config.mak
include $(ROOT)/mak/configschema.mak

# Loads the sample MOF into a separate namespace.
# The following variable determine the version of the Schema to
# be loaded into the Sample Provider namespace.
# Update the following variable to change the version.

SAMPLEPROVIDERNS=root/SampleProvider
SAMPLEPROVIDERNSDIRECTORY=$(REPOSITORY_ROOT)/root\#SampleProvider




depend:

sub:

misc:

tests:

messages:

poststarttests:

	@$(ECHO) +++++ Enumerating Sample_InstanceQueryProviderClass $(SAMPLEPROVIDERNS)

	CLI ei Sample_InstanceQueryProviderClass "-n $(SAMPLEPROVIDERNS)" -s -count 3

	@$(ECHO) +++++ Querying Sample_InstanceQueryProviderClass $(SAMPLEPROVIDERNS)
	CLI xq "select * from Sample_InstanceQueryProviderClass where identifier=2 or idetifier=3" \
	 "-n $(SAMPLEPROVIDERNS)" -s -count 2



general:

clean:
# END_OF_FILE
