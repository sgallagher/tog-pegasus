# Test the Family Provider using the script toosl
# ATTN: Today this makefile is implementation dependent.
#       It assumes that the repository is a set of directories
#       that can be removed by removing the directories.
#       This must be updated to an implementation independent repository
#       remove function.

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


poststarttests:

	@ echo +++++ TST_Person Testing Family provider in  $(SAMPLEPROVIDERNS)

	CLI ei TST_Person -n $(SAMPLEPROVIDERNS) -s -count 9
	CLI ni TST_Person -n $(SAMPLEPROVIDERNS) -s -count 9
	CLI rn TST_Person -n $(SAMPLEPROVIDERNS) -s -count 24
	CLI r  TST_Person -n $(SAMPLEPROVIDERNS) -s -count 24
	CLI an TST_Person -n $(SAMPLEPROVIDERNS) -s -count 8
	CLI a  TST_Person -n $(SAMPLEPROVIDERNS) -s -count 8
	CLI rn TST_Person.name=@Mike@ -n $(SAMPLEPROVIDERNS) -s -count 1
	CLI r  TST_Person.name=@Mike@ -n $(SAMPLEPROVIDERNS) -s -count 1
	CLI an TST_Person.name=@Mike@ -n $(SAMPLEPROVIDERNS) -s -count 1
	CLI a  TST_Person.name=@Mike@ -n $(SAMPLEPROVIDERNS) -s -count 1
	
 	@ echo +++++ TST_PersonDynamic Testing Family provider in  $(SAMPLEPROVIDERNS)
   
	CLI ei TST_PersonDynamic -n $(SAMPLEPROVIDERNS) -s -count 5
	CLI ni TST_PersonDynamic -n $(SAMPLEPROVIDERNS) -s -count 5
	CLI rn TST_PersonDynamic -n $(SAMPLEPROVIDERNS) -s -count 25
	CLI r  TST_PersonDynamic -n $(SAMPLEPROVIDERNS) -s -count 25
	CLI an TST_PersonDynamic -n $(SAMPLEPROVIDERNS) -s -count 2
	CLI a  TST_PersonDynamic -n $(SAMPLEPROVIDERNS) -s -count 2				 
	CLI rn TST_PersonDynamic.name=@Father@ -n $(SAMPLEPROVIDERNS) -s -count 6
	CLI r TST_PersonDynamic.name=@Father@ -n $(SAMPLEPROVIDERNS) -s -count 6
	CLI an TST_PersonDynamic.name=@Father@ -n $(SAMPLEPROVIDERNS) -s -count 9
	CLI a TST_PersonDynamic.name=@Father@ -n $(SAMPLEPROVIDERNS) -s -count 27



general:

clean:

