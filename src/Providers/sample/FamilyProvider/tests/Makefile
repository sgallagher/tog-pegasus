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

	@echo +++++ TST_Person Testing Family provider in  $(SAMPLEPROVIDERNS)

	CLI ei TST_Person -n $(SAMPLEPROVIDERNS) -s -count 13
	CLI ni TST_Person -n $(SAMPLEPROVIDERNS) -s -count 13
	CLI rn TST_Person -n $(SAMPLEPROVIDERNS) -s -count 4
	CLI r  TST_Person -n $(SAMPLEPROVIDERNS) -s -count 4
	CLI an TST_Person -n $(SAMPLEPROVIDERNS) -s -count 1
	CLI a  TST_Person -n $(SAMPLEPROVIDERNS) -s -count 1
	CLI rn TST_Person.name=@Mike@ -n $(SAMPLEPROVIDERNS) -s -count 2
	CLI r  TST_Person.name=@Mike@ -n $(SAMPLEPROVIDERNS) -s -count 2
	CLI an TST_Person.name=@Mike@ -n $(SAMPLEPROVIDERNS) -s -count 2
	CLI a  TST_Person.name=@Mike@ -n $(SAMPLEPROVIDERNS) -s -count 2

	@echo +++++ TST_Person Testing with  resultClass filters
    
	CLI rn TST_Person -n $(SAMPLEPROVIDERNS)  -rc TST_Lineage -s -count 1
	CLI r TST_Person -n $(SAMPLEPROVIDERNS)  -rc TST_Lineage -s -count 1
	CLI rn TST_Person.name=@Mike@ -n $(SAMPLEPROVIDERNS) -rc TST_Lineage -s -count 2
	CLI r  TST_Person.name=@Mike@ -n $(SAMPLEPROVIDERNS) -rc TST_Lineage -s -count 2

	@echo ++++ TST_Person Testing with assocClass filter

	CLI an TST_Person -n $(SAMPLEPROVIDERNS)  -ac  TST_Lineage -s -count 1
	CLI a TST_Person -n $(SAMPLEPROVIDERNS)  -ac TST_Lineage -s -count 1
	CLI an TST_Person.name=@Mike@ -n $(SAMPLEPROVIDERNS) -ac TST_Lineage -s -count 2
	CLI a  TST_Person.name=@Mike@ -n $(SAMPLEPROVIDERNS) -ac TST_Lineage -s -count 2

	@echo ++++ TST_Person Testing with role filter alone

	CLI rn TST_Person -n $(SAMPLEPROVIDERNS) -r parent -s -count 4
	CLI r TST_Person -n $(SAMPLEPROVIDERNS)  -r parent -s -count 4
	CLI rn TST_Person.name=@Mike@ -n $(SAMPLEPROVIDERNS) -r parent -s -count 2
	CLI r  TST_Person.name=@Mike@ -n $(SAMPLEPROVIDERNS) -r parent -s -count 2
	
	CLI rn TST_Person -n $(SAMPLEPROVIDERNS) -r child -s -count 4
	CLI r TST_Person -n $(SAMPLEPROVIDERNS)  -r child -s -count 4
	CLI rn TST_Person.name=@Mike@ -n $(SAMPLEPROVIDERNS) -r child -s -count 0
	CLI r  TST_Person.name=@Mike@ -n $(SAMPLEPROVIDERNS) -r child -s -count 0

	@echo +++++ TEST_Person Test with role and result class

	CLI rn TST_Person -n $(SAMPLEPROVIDERNS) -rc  TST_Lineage -r parent -s -count 1
	CLI r TST_Person -n $(SAMPLEPROVIDERNS)  -rc  TST_Lineage -r parent -s -count 1
	CLI rn TST_Person.name=@Mike@ -n $(SAMPLEPROVIDERNS) -rc  TST_Lineage -r parent -s -count 2
	CLI r  TST_Person.name=@Mike@ -n $(SAMPLEPROVIDERNS) -rc  TST_Lineage -r parent -s -count 2

	@echo ++++ TST_Person Test with errors in role and result class

	CLI rn TST_Person -n $(SAMPLEPROVIDERNS) -rc  TST_Lineage -r junk -s -count 0
	CLI r TST_Person -n $(SAMPLEPROVIDERNS)  -rc  TST_Lineage -r junk -s -count 0
# bypassing the two tests below because they generate error response stopping test.
# The error is valid, Junk is bad param. Note that repository does not generate same error.
#TEMPBypass	CLI rn TST_Person.name=@Mike@ -n $(SAMPLEPROVIDERNS) -rc  TST_Lineage -r junk -s -count 0
#TEMPBypass	CLI r  TST_Person.name=@Mike@ -n $(SAMPLEPROVIDERNS) -rc  TST_Lineage -r junk -s -count 0

	CLI rn TST_Person -n $(SAMPLEPROVIDERNS) -rc  TST_Lineage -r junk -s -count 0
	CLI r TST_Person -n $(SAMPLEPROVIDERNS)  -rc  TST_Lineage -r junk -s -count 0
# Bypassing following two tests because they generate exception return from provider
# The error is valid because Provider does exception on bad property.
#	CLI rn TST_Person.name=@Mike@ -n $(SAMPLEPROVIDERNS) -rc  TST_Lineage -r junk -s -count 0
#	CLI r  TST_Person.name=@Mike@ -n $(SAMPLEPROVIDERNS) -rc  TST_Lineage -r junk -s -count 0

	@echo add tests for associations with  result class and result role.

# The following actually generates a CIMException. Investigate why KS 20030608.
#	CLI an  TST_Person.name=@Mike@ -n $(SAMPLEPROVIDERNS) -rc  TST_Lineage -r -s -count 0
#	CLI an  TST_Person.name=@Mike@ -n $(SAMPLEPROVIDERNS) -rc  TST_Lineage -r junk -s -count 0

	@echo +++++ TST_Person with result Class filters but with bad Class
#To Be Done.

	@echo +++++ TST_PersonDynamic Family class Operations in  $(SAMPLEPROVIDERNS)

	CLI ei TST_PersonDynamic -n $(SAMPLEPROVIDERNS) -s -count 5
	CLI ni TST_PersonDynamic -n $(SAMPLEPROVIDERNS) -s -count 5
	CLI rn TST_PersonDynamic -n $(SAMPLEPROVIDERNS) -s -count 5
	CLI r  TST_PersonDynamic -n $(SAMPLEPROVIDERNS) -s -count 5
	CLI an TST_PersonDynamic -n $(SAMPLEPROVIDERNS) -s -count 2
	CLI a  TST_PersonDynamic -n $(SAMPLEPROVIDERNS) -s -count 2

	@echo +++++ TST_PersonDynamic and Family provider with simple class operations
 
	CLI rn TST_PersonDynamic.name=@Father@ -n $(SAMPLEPROVIDERNS) -s -count 3
	CLI r TST_PersonDynamic.name=@Father@ -n $(SAMPLEPROVIDERNS) -s -count 3
	CLI an TST_PersonDynamic.name=@Father@ -n $(SAMPLEPROVIDERNS) -s -count 3
	CLI a TST_PersonDynamic.name=@Father@ -n $(SAMPLEPROVIDERNS) -s -count 3

	@echo "TST_PersonDynamic with bad object name (lower case)"

	CLI rn TST_PersonDynamic.name=@father@ -n $(SAMPLEPROVIDERNS) -s -count 0

	@echo Test TST_PersonDynamic classes with  resultClass/AssocClass options

	CLI rn TST_PersonDynamic -n $(SAMPLEPROVIDERNS) -rc TST_LineageDynamic -s -count 1
	CLI r TST_PersonDynamic -n $(SAMPLEPROVIDERNS) -rc TST_LineageDynamic -s -count 1
	CLI an TST_PersonDynamic -n $(SAMPLEPROVIDERNS) -ac TST_LineageDynamic -s -count 1
	CLI a TST_PersonDynamic -n $(SAMPLEPROVIDERNS) -ac TST_LineageDynamic -s -count 1

	@echo TST_PersonDynamic instances with resultClass/AssociationClass options

	CLI rn TST_PersonDynamic.name=@Father@ -n $(SAMPLEPROVIDERNS) -rc TST_LineageDynamic -s -count 3
	CLI r TST_PersonDynamic.name=@Father@ -n $(SAMPLEPROVIDERNS) -rc TST_LineageDynamic -s -count 3
	CLI an TST_PersonDynamic.name=@Father@ -n $(SAMPLEPROVIDERNS) -ac TST_LineageDynamic -s -count 3
	CLI a TST_PersonDynamic.name=@Father@ -n $(SAMPLEPROVIDERNS) -ac TST_LineageDynamic -s -count 3
	CLI a TST_PersonDynamic.name=@Father@ -n $(SAMPLEPROVIDERNS) -ac TST_LineageDynamic -s -count 3

	@echo TST_PersonDynamic association with resultClass role option

	CLI an TST_PersonDynamic -n $(SAMPLEPROVIDERNS) -rc TST_Lineage -s -count 0
	CLI an TST_PersonDynamic -n $(SAMPLEPROVIDERNS) -rc TST_PersonDynamic -s -count 1
	CLI an TST_PersonDynamic -n $(SAMPLEPROVIDERNS) -rc TST_Person -s -count 2


general:

clean:
# END_OF_FILE
