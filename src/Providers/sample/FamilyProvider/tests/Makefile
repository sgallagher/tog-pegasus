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

	echo +++++ TST_Person Testing Family provider in  $(SAMPLEPROVIDERNS)

	CLI ei TST_Person -n $(SAMPLEPROVIDERNS) -s -count 9
	CLI ni TST_Person -n $(SAMPLEPROVIDERNS) -s -count 9
	CLI rn TST_Person -n $(SAMPLEPROVIDERNS) -s -count 4
	CLI r  TST_Person -n $(SAMPLEPROVIDERNS) -s -count 4
	CLI an TST_Person -n $(SAMPLEPROVIDERNS) -s -count 8
	CLI a  TST_Person -n $(SAMPLEPROVIDERNS) -s -count 8
	CLI rn TST_Person.name=@Mike@ -n $(SAMPLEPROVIDERNS) -s -count 1
	CLI r  TST_Person.name=@Mike@ -n $(SAMPLEPROVIDERNS) -s -count 1
	CLI an TST_Person.name=@Mike@ -n $(SAMPLEPROVIDERNS) -s -count 1
	CLI a  TST_Person.name=@Mike@ -n $(SAMPLEPROVIDERNS) -s -count 1
	
	echo +++++ TST_Person Testing with  result class filters
    
	CLI rn TST_Person -n $(SAMPLEPROVIDERNS)  -rc  TST_Lineage -s -count 9
	CLI r TST_Person -n $(SAMPLEPROVIDERNS)  -rc TST_Lineage -s -count 9
	CLI rn TST_Person.name=@Mike@ -n $(SAMPLEPROVIDERNS) -rc TST_Lineage -s -count 1
	CLI r  TST_Person.name=@Mike@ -n $(SAMPLEPROVIDERNS) -rc TST_Lineage -s -count 1

	echo ++++ TST_Person Testing with assocClass filter
	
	CLI an TST_Person -n $(SAMPLEPROVIDERNS)  -rc  TST_Lineage -s -count 9
	CLI a TST_Person -n $(SAMPLEPROVIDERNS)  -rc TST_Lineage -s -count 9
	CLI an TST_Person.name=@Mike@ -n $(SAMPLEPROVIDERNS) -rc TST_Lineage -s -count 1
	CLI a  TST_Person.name=@Mike@ -n $(SAMPLEPROVIDERNS) -rc TST_Lineage -s -count 1
	
	echo ++++ TST_Person Teting with role filter alone
	
	CLI rn TST_Person -n $(SAMPLEPROVIDERNS) -role parent -s -count 9
	CLI r TST_Person -n $(SAMPLEPROVIDERNS)  -role parent -s -count 9
	CLI rn TST_Person.name=@Mike@ -n $(SAMPLEPROVIDERNS) -r parent -s -count 1
	CLI r  TST_Person.name=@Mike@ -n $(SAMPLEPROVIDERNS) -r parent -s -count 1
	
	CLI rn TST_Person -n $(SAMPLEPROVIDERNS) -role child -s -count 9
	CLI r TST_Person -n $(SAMPLEPROVIDERNS)  -role child -s -count 9
	CLI rn TST_Person.name=@Mike@ -n $(SAMPLEPROVIDERNS) -r child -s -count 1
	CLI r  TST_Person.name=@Mike@ -n $(SAMPLEPROVIDERNS) -r child -s -count 1
	
	echo +++++ TEST_Person Test with role and result class
	
	CLI rn TST_Person -n $(SAMPLEPROVIDERNS) -rc  TST_Lineage -role parent -s -count 9
	CLI r TST_Person -n $(SAMPLEPROVIDERNS)  -rc  TST_Lineage -role parent -s -count 9
	CLI rn TST_Person.name=@Mike@ -n $(SAMPLEPROVIDERNS) -rc  TST_Lineage -r parent -s -count 1
	CLI r  TST_Person.name=@Mike@ -n $(SAMPLEPROVIDERNS) -rc  TST_Lineage -r parent -s -count 1
	
	echo ++++ TST_Person Test with errors in role and result class
	CLI rn TST_Person -n $(SAMPLEPROVIDERNS) -rc  TST_Lineage -r junk -s -count 0
	CLI r TST_Person -n $(SAMPLEPROVIDERNS)  -rc  TST_Lineage -r junk -s -count 0
	CLI rn TST_Person.name=@Mike@ -n $(SAMPLEPROVIDERNS) -rc  TST_Lineage -r junk -s -count 0
	CLI r  TST_Person.name=@Mike@ -n $(SAMPLEPROVIDERNS) -rc  TST_Lineage -r junk -s -count 0
	
	CLI rn TST_Person -n $(SAMPLEPROVIDERNS) -rc  TST_Lineage -r junk -s -count 9
	CLI r TST_Person -n $(SAMPLEPROVIDERNS)  -rc  TST_Lineage -r junk -s -count 9
	CLI rn TST_Person.name=@Mike@ -n $(SAMPLEPROVIDERNS) -rc  TST_Lineage -r junk -s -count 0
	CLI r  TST_Person.name=@Mike@ -n $(SAMPLEPROVIDERNS) -rc  TST_Lineage -r junk -s -count 0
	
     echo add tests for associations with  result class and result role.
	CLI an  TST_Person.name=@Mike@ -n $(SAMPLEPROVIDERNS) -rc  TST_Lineage -r -s -count 0
	
	

	echo +++++ TST_Person Testing with result Class filters but with bad
	
	echo +++++ TST_PersonDynamic Testing Family provider in  $(SAMPLEPROVIDERNS)
	CLI ei TST_PersonDynamic -n $(SAMPLEPROVIDERNS) -s -count 5
	CLI ni TST_PersonDynamic -n $(SAMPLEPROVIDERNS) -s -count 5
	CLI rn TST_PersonDynamic -n $(SAMPLEPROVIDERNS) -s -count 5
	CLI r  TST_PersonDynamic -n $(SAMPLEPROVIDERNS) -s -count 5
	CLI an TST_PersonDynamic -n $(SAMPLEPROVIDERNS) -s -count 2
	CLI a  TST_PersonDynamic -n $(SAMPLEPROVIDERNS) -s -count 2 
	CLI rn TST_PersonDynamic.name=@Father@ -n $(SAMPLEPROVIDERNS) -s -count 3
	CLI r TST_PersonDynamic.name=@Father@ -n $(SAMPLEPROVIDERNS) -s -count 3
	CLI an TST_PersonDynamic.name=@Father@ -n $(SAMPLEPROVIDERNS) -s -count 3
	CLI a TST_PersonDynamic.name=@Father@ -n $(SAMPLEPROVIDERNS) -s -count 3

	echo Test with bad name (lower case)
	CLI rn TST_PersonDynamic.name=@father@ -n $(SAMPLEPROVIDERNS) -s -count 0

	echo Test resultClass resultClass/AssocClassoptions
	CLI rn TST_PersonDynamic -n $(SAMPLEPROVIDERNS) -rc TST_LineageDynamic -s -count 5
	CLI r TST_PersonDynamic -n $(SAMPLEPROVIDERNS) -rc TST_LineageDynamic -s -count 5
	CLI an TST_PersonDynamic -n $(SAMPLEPROVIDERNS) -ac TST_LineageDynamic -s -count 5
	CLI a TST_PersonDynamic -n $(SAMPLEPROVIDERNS) -ac TST_LineageDynamic -s -count 5
	CLI rn TST_PersonDynamic.name=@Father@ -n $(SAMPLEPROVIDERNS) -rc TST_LineageDynamic -s -count 5
	CLI r TST_PersonDynamic.name=@Father@ -n $(SAMPLEPROVIDERNS) -rc TST_LineageDynamic -s -count 5
	CLI an TST_PersonDynamic.name=@Father@ -n $(SAMPLEPROVIDERNS) -ac TST_LineageDynamic -s -count 5
	CLI a TST_PersonDynamic.name=@Father@ -n $(SAMPLEPROVIDERNS) -ac TST_LineageDynamic -s -count 5
	
	echo Test resultClass role option
	CLI rn TST_PersonDynamic -n $(SAMPLEPROVIDERNS) -rc TST_LineageDynamic -s -count 5
	
	
general:

clean:

