# Test the Family Provider using the script toosls

ROOT = $(PEGASUS_ROOT)
include $(ROOT)/mak/config.mak
include $(ROOT)/mak/configschema.mak
SHH = @

# sampleprovider\Load puts the sample MOF into a separate namespace.
# The following variable defines this namespace. Note that
# originally this Makefile was to do the install but that
# was moved to sampleprovider/load

SAMPLEPROVIDERNS=root/SampleProvider

depend:

sub:

misc:

tests:

messages:

poststarttests:

	@$(ECHO) 1. +++++ Test Classes to be sure all classes are here

	$(SHH)CLI ec "TST_Person" -n "$(SAMPLEPROVIDERNS)" -s -count 2
	$(SHH)CLI ec "TST_Person" -n "$(SAMPLEPROVIDERNS)" -s -di -count 3
	$(SHH)CLI ec "TST_PersonS" -n "$(SAMPLEPROVIDERNS)" -s -count 0
	$(SHH)CLI ec "TST_PersonDynamic" -n "$(SAMPLEPROVIDERNS)" -s -count 1
	$(SHH)CLI ec "TST_Lineage" -n "$(SAMPLEPROVIDERNS)" -s -count 0
	$(SHH)CLI ec "TST_LineageDynamic" -n "$(SAMPLEPROVIDERNS)" -s -count 0
	@$(ECHO) 2. +++++ Test all enumerate operations on the classes
	
	$(SHH)CLI ei "TST_Person" -n "$(SAMPLEPROVIDERNS)" -s -count 14
	$(SHH)CLI ni "TST_Person" -n "$(SAMPLEPROVIDERNS)" -s -count 14
	$(SHH)CLI ei "TST_PersonS" -n "$(SAMPLEPROVIDERNS)" -s -count 4
	$(SHH)CLI ni "TST_PersonS" -n "$(SAMPLEPROVIDERNS)" -s -count 4
	$(SHH)CLI ei "TST_PersonDynamic" -n "$(SAMPLEPROVIDERNS)" -s -count 6
	$(SHH)CLI ni "TST_PersonDynamic" -n "$(SAMPLEPROVIDERNS)" -s -count 6

	$(SHH)CLI ei "TST_PersonDynamicSubclass" -n "$(SAMPLEPROVIDERNS)" -s -count 1
	$(SHH)CLI ni "TST_PersonDynamicSubClass" -n "$(SAMPLEPROVIDERNS)" -s -count 1
	
	$(SHH)CLI ei "TST_Lineage" -n "$(SAMPLEPROVIDERNS)" -s -count 4
	$(SHH)CLI ni "TST_Lineage" -n "$(SAMPLEPROVIDERNS)" -s -count 4
	$(SHH)CLI ei "TST_LabeledLineage" -n "$(SAMPLEPROVIDERNS)" -s -count 0
	$(SHH)CLI ni "TST_LabeledLineage" -n "$(SAMPLEPROVIDERNS)" -s -count 0

	$(SHH)CLI ei "TST_LineageDynamic" -n "$(SAMPLEPROVIDERNS)" -s -count 6
	$(SHH)CLI ni "TST_LineageDynamic" -n "$(SAMPLEPROVIDERNS)" -s -count 6
	
	$(SHH)CLI ei "TST_LabeledLineageDynamic" -n "$(SAMPLEPROVIDERNS)" -s -count 6
	$(SHH)CLI ni "TST_LabeledLineageDynamic" -n "$(SAMPLEPROVIDERNS)" -s -count 6
	
	@$(ECHO) 3. +++++ TST_Person Testing Family provider in  $(SAMPLEPROVIDERNS)

	$(SHH)CLI ei "TST_Person" -n "$(SAMPLEPROVIDERNS)" -s -count 14
	$(SHH)CLI ni "TST_Person" -n "$(SAMPLEPROVIDERNS)" -s -count 14
	$(SHH)CLI rn "TST_Person" -n "$(SAMPLEPROVIDERNS)" -s -count 4
	$(SHH)CLI r  "TST_Person" -n "$(SAMPLEPROVIDERNS)" -s -count 4
	$(SHH)CLI an "TST_Person" -n "$(SAMPLEPROVIDERNS)" -s -count 1
	$(SHH)CLI a  "TST_Person" -n "$(SAMPLEPROVIDERNS)" -s -count 1
	$(SHH)CLI rn "TST_Person.name=@Mike@" -n "$(SAMPLEPROVIDERNS)" -s -count 2
	$(SHH)CLI r  "TST_Person.name=@Mike@" -n "$(SAMPLEPROVIDERNS)" -s -count 2
	$(SHH)CLI an "TST_Person.name=@Mike@" -n "$(SAMPLEPROVIDERNS)" -s -count 2
	$(SHH)CLI a  "TST_Person.name=@Mike@" -n "$(SAMPLEPROVIDERNS)" -s -count 2

	@$(ECHO) 4. +++++ TST_Person Testing with  resultClass filters
    
	$(SHH)CLI rn "TST_Person" -n "$(SAMPLEPROVIDERNS)" -rc "TST_Lineage" -s -count 1
	$(SHH)CLI r "TST_Person" -n "$(SAMPLEPROVIDERNS)" -rc "TST_Lineage" -s -count 1
	$(SHH)CLI rn "TST_Person.name=@Mike@" -n "$(SAMPLEPROVIDERNS)" -rc "TST_Lineage" -s -count 2
	$(SHH)CLI r  "TST_Person.name=@Mike@" -n "$(SAMPLEPROVIDERNS)" -rc "TST_Lineage" -s -count 2

	@$(ECHO) 5. ++++ TST_Person Testing with assocClass filter

	$(SHH)CLI an "TST_Person" -n "$(SAMPLEPROVIDERNS)" -ac  "TST_Lineage" -s -count 1
	$(SHH)CLI a "TST_Person" -n "$(SAMPLEPROVIDERNS)" -ac "TST_Lineage" -s -count 1
	$(SHH)CLI an "TST_Person.name=@Mike@" -n "$(SAMPLEPROVIDERNS)" -ac "TST_Lineage" -s -count 2
	$(SHH)CLI a  "TST_Person.name=@Mike@" -n "$(SAMPLEPROVIDERNS)" -ac "TST_Lineage" -s -count 2

	@$(ECHO) 6. ++++ TST_Person Testing with role filter alone

	$(SHH)CLI rn "TST_Person" -n "$(SAMPLEPROVIDERNS)" -r parent -s -count 4
	$(SHH)CLI r "TST_Person" -n "$(SAMPLEPROVIDERNS)" -r parent -s -count 4
	$(SHH)CLI rn "TST_Person.name=@Mike@" -n "$(SAMPLEPROVIDERNS)" -r parent -s -count 2
	$(SHH)CLI r  "TST_Person.name=@Mike@" -n "$(SAMPLEPROVIDERNS)" -r parent -s -count 2
	
	$(SHH)CLI rn "TST_Person" -n "$(SAMPLEPROVIDERNS)" -r child -s -count 4
	$(SHH)CLI r "TST_Person" -n "$(SAMPLEPROVIDERNS)" -r child -s -count 4
	$(SHH)CLI rn "TST_Person.name=@Mike@" -n "$(SAMPLEPROVIDERNS)" -r child -s -count 0
	$(SHH)CLI r  "TST_Person.name=@Mike@" -n "$(SAMPLEPROVIDERNS)" -r child -s -count 0

	@$(ECHO) 7. +++++ TEST_Person Test with role and result class

	$(SHH)CLI rn "TST_Person" -n "$(SAMPLEPROVIDERNS)" -rc  TST_Lineage -r parent -s -count 1
	$(SHH)CLI r "TST_Person" -n "$(SAMPLEPROVIDERNS)" -rc  TST_Lineage -r parent -s -count 1
	$(SHH)CLI rn "TST_Person.name=@Mike@" -n "$(SAMPLEPROVIDERNS)" -rc  "TST_Lineage" -r parent -s -count 2
	$(SHH)CLI r  "TST_Person.name=@Mike@" -n "$(SAMPLEPROVIDERNS)" -rc  "TST_Lineage" -r parent -s -count 2

	@$(ECHO) 8. ++++ TST_Person Test with errors in role and result class

	$(SHH)CLI rn "TST_Person" -n "$(SAMPLEPROVIDERNS)" -rc  "TST_Lineage" -r junk -s -count 0
	$(SHH)CLI r "TST_Person" -n "$(SAMPLEPROVIDERNS)" -rc  "TST_Lineage" -r junk -s -count 0
	$(SHH)CLI rn TST_Person.name=@Mike@ -n "$(SAMPLEPROVIDERNS)" -rc  TST_Lineage -r junk -s -count 0
	$(SHH)CLI r  TST_Person.name=@Mike@ -n "$(SAMPLEPROVIDERNS)" -rc  TST_Lineage -r junk -s -count 0

	$(SHH)CLI rn "TST_Person" -n "$(SAMPLEPROVIDERNS)" -rc  "TST_Lineage" -r junk -s -count 0
	$(SHH)CLI r "TST_Person" -n "$(SAMPLEPROVIDERNS)" -rc  "TST_Lineage" -r junk -s -count 0
# Bypassing following two tests because they generate exception return from provider
# The error is valid because Provider does exception on bad property.
#   But we don't know how to handle that in a script with CLI
#	CLI rn "TST_Person.name=@Mike@" -n "$(SAMPLEPROVIDERNS)" -rc  "TST_Lineage" -r junk -s -count 0
#	CLI r  "TST_Person.name=@Mike@" -n "$(SAMPLEPROVIDERNS)" -rc  "TST_Lineage" -r junk -s -count 0

	@$(ECHO) 9. ++++ add tests for associations with  result class and result role.

	$(SHH)CLI an  "TST_Person.name=@Mike@" -n "$(SAMPLEPROVIDERNS)" -rc  "TST_Lineage" -r -s -count 0
	$(SHH)CLI an  "TST_Person.name=@Mike@" -n "$(SAMPLEPROVIDERNS)" -rc  "TST_Lineage" -r junk -s -count 0

	@$(ECHO) 10. +++++ TST_Person with result Class filters but with bad Class
#Note that we can add more tests here.

	@$(ECHO) 11. +++++ TST_PersonDynamic Family class Operations in  $(SAMPLEPROVIDERNS)

	$(SHH)CLI ei "TST_PersonDynamic" -n "$(SAMPLEPROVIDERNS)" -s -count 6
	$(SHH)CLI ni "TST_PersonDynamic" -n "$(SAMPLEPROVIDERNS)" -s -count 6
	$(SHH)CLI rn "TST_PersonDynamic" -n "$(SAMPLEPROVIDERNS)" -s -count 5
	$(SHH)CLI r  "TST_PersonDynamic" -n "$(SAMPLEPROVIDERNS)" -s -count 5
	$(SHH)CLI an "TST_PersonDynamic" -n "$(SAMPLEPROVIDERNS)" -s -count 2
	$(SHH)CLI a  "TST_PersonDynamic" -n "$(SAMPLEPROVIDERNS)" -s -count 2

	@$(ECHO) 12. +++++ TST_PersonDynamic and Family provider with simple class operations
 
	$(SHH)CLI rn "TST_PersonDynamic.name=@Father@" -n "$(SAMPLEPROVIDERNS)" -s -count 6
	$(SHH)CLI r "TST_PersonDynamic.name=@Father@" -n "$(SAMPLEPROVIDERNS)" -s -count 6
	$(SHH)CLI an "TST_PersonDynamic.name=@Father@" -n "$(SAMPLEPROVIDERNS)" -s -count 6
	$(SHH)CLI a "TST_PersonDynamic.name=@Father@" -n "$(SAMPLEPROVIDERNS)" -s -count 6

	@$(ECHO) "TST_PersonDynamic with bad object name (lower case)"

	$(SHH)CLI rn "TST_PersonDynamic.name=@father@" -n "$(SAMPLEPROVIDERNS)" -s -count 0

	@$(ECHO) 13. ++++ Test TST_PersonDynamic classes with  resultClass/AssocClass options

	$(SHH)CLI rn "TST_PersonDynamic" -n "$(SAMPLEPROVIDERNS)" -rc "TST_LineageDynamic" -s -count 1
	$(SHH)CLI r "TST_PersonDynamic" -n "$(SAMPLEPROVIDERNS)" -rc "TST_LineageDynamic" -s -count 1
	$(SHH)CLI an "TST_PersonDynamic" -n "$(SAMPLEPROVIDERNS)" -ac "TST_LineageDynamic" -s -count 1
	$(SHH)CLI a "TST_PersonDynamic" -n "$(SAMPLEPROVIDERNS)" -ac "TST_LineageDynamic" -s -count 1

	@$(ECHO) 14. ++++ TST_PersonDynamic instances with resultClass/AssociationClass options

	$(SHH)CLI rn "TST_PersonDynamic.name=@Father@" -n "$(SAMPLEPROVIDERNS)" -rc "TST_LineageDynamic" -s -count 3
	$(SHH)CLI r "TST_PersonDynamic.name=@Father@" -n "$(SAMPLEPROVIDERNS)" -rc "TST_LineageDynamic" -s -count 3
	$(SHH)CLI an "TST_PersonDynamic.name=@Father@" -n "$(SAMPLEPROVIDERNS)" -ac "TST_LineageDynamic" -s -count 3
	$(SHH)CLI a "TST_PersonDynamic.name=@Father@" -n "$(SAMPLEPROVIDERNS)" -ac "TST_LineageDynamic" -s -count 3
	$(SHH)CLI a "TST_PersonDynamic.name=@Father@" -n "$(SAMPLEPROVIDERNS)" -ac "TST_LineageDynamic" -s -count 3

	@$(ECHO) 15. ++++ TST_PersonDynamic association with resultClass role option

	$(SHH)CLI an "TST_PersonDynamic" -n "$(SAMPLEPROVIDERNS)" -rc "TST_Lineage" -s -count 0
	$(SHH)CLI an "TST_PersonDynamic" -n "$(SAMPLEPROVIDERNS)" -rc "TST_PersonDynamic" -s -count 1
	$(SHH)CLI an "TST_PersonDynamic" -n "$(SAMPLEPROVIDERNS)" -rc "TST_Person" -s -count 2

general:

clean:

install:
# END_OF_FILE
