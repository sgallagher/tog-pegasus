#//%2006////////////////////////////////////////////////////////////////////////
#//
#// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
#// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
#// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation, The Open Group.
#// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
#// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
#// EMC Corporation; VERITAS Software Corporation; The Open Group.
#// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
#// EMC Corporation; Symantec Corporation; The Open Group.
#//
#// Permission is hereby granted, free of charge, to any person obtaining a copy
#// of this software and associated documentation files (the "Software"), to
#// deal in the Software without restriction, including without limitation the
#// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
#// sell copies of the Software, and to permit persons to whom the Software is
#// furnished to do so, subject to the following conditions:
#// 
#// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
#// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
#// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
#// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
#// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
#// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
#// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
#// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#//
#//==============================================================================
# Test the Family Provider using the script toosls

ROOT = $(PEGASUS_ROOT)
include $(ROOT)/mak/config.mak
include $(ROOT)/mak/configschema.mak
SHH = @

# sampleprovider\Load puts the sample MOF into a separate namespace.
# The following variable defines this namespace. Note that
# originally this Makefile was to do the install but that
# was moved to sampleprovider/load

PROVIDERNS=test/TestProvider

depend:

sub:

misc:

tests:

messages:

poststarttests:

	@$(ECHO) 1. +++++ Test Classes to be sure all classes are here

	$(SHH)CLI ec "TST_Person" -n "$(PROVIDERNS)" --sum -count 2
	$(SHH)CLI ec "TST_Person" -n "$(PROVIDERNS)" --sum -di -count 3
	$(SHH)CLI ec "TST_PersonS" -n "$(PROVIDERNS)" --sum -count 0
	$(SHH)CLI ec "TST_PersonDynamic" -n "$(PROVIDERNS)" --sum -count 1
	$(SHH)CLI ec "TST_Lineage" -n "$(PROVIDERNS)" --sum -count 0
	$(SHH)CLI ec "TST_LineageDynamic" -n "$(PROVIDERNS)" --sum -count 0

	@$(ECHO) 2. +++++ Test all enumerate operations on the classes

	$(SHH)CLI ei "TST_Person" -n "$(PROVIDERNS)" --sum -count 14
	$(SHH)CLI ni "TST_Person" -n "$(PROVIDERNS)" --sum -count 14
	$(SHH)CLI ei "TST_PersonS" -n "$(PROVIDERNS)" --sum -count 4
	$(SHH)CLI ni "TST_PersonS" -n "$(PROVIDERNS)" --sum -count 4
	$(SHH)CLI ei "TST_PersonDynamic" -n "$(PROVIDERNS)" --sum -count 6
	$(SHH)CLI ni "TST_PersonDynamic" -n "$(PROVIDERNS)" --sum -count 6

	$(SHH)CLI ei "TST_PersonDynamicSubclass" -n "$(PROVIDERNS)" --sum -count 1
	$(SHH)CLI ni "TST_PersonDynamicSubClass" -n "$(PROVIDERNS)" --sum -count 1

	$(SHH)CLI ei "TST_Lineage" -n "$(PROVIDERNS)" --sum -count 4
	$(SHH)CLI ni "TST_Lineage" -n "$(PROVIDERNS)" --sum -count 4
	$(SHH)CLI ei "TST_LabeledLineage" -n "$(PROVIDERNS)" --sum -count 0
	$(SHH)CLI ni "TST_LabeledLineage" -n "$(PROVIDERNS)" --sum -count 0

	$(SHH)CLI ei "TST_LineageDynamic" -n "$(PROVIDERNS)" --sum -count 6
	$(SHH)CLI ni "TST_LineageDynamic" -n "$(PROVIDERNS)" --sum -count 6

	$(SHH)CLI ei "TST_LabeledLineageDynamic" -n "$(PROVIDERNS)" --sum -count 6
	$(SHH)CLI ni "TST_LabeledLineageDynamic" -n "$(PROVIDERNS)" --sum -count 6

	@$(ECHO) 3. +++++ TST_Person Testing getInstance Family provider in  $(PROVIDERNS)

	$(SHH)CLI gi "TST_Person.name=\"Mike\"" -n "$(PROVIDERNS)" --sum -count 1
	$(SHH)CLI gi "TST_PersonDynamicSubClass.name=\"AnotherKid\"" -n "$(PROVIDERNS)" --sum -count 1
	$(SHH)CLI gi "TST_PersonDynamic.name=\"Father\"" -n "$(PROVIDERNS)" --sum -count 1

	@$(ECHO) 4. +++++ TST_Person Testing Family provider in  $(PROVIDERNS)

	$(SHH)CLI ei "TST_Person" -n "$(PROVIDERNS)" --sum -count 14
	$(SHH)CLI ni "TST_Person" -n "$(PROVIDERNS)" --sum -count 14
	$(SHH)CLI rn "TST_Person" -n "$(PROVIDERNS)" --sum -count 4
	$(SHH)CLI r  "TST_Person" -n "$(PROVIDERNS)" --sum -count 4
	$(SHH)CLI an "TST_Person" -n "$(PROVIDERNS)" --sum -count 1
	$(SHH)CLI a  "TST_Person" -n "$(PROVIDERNS)" --sum -count 1
	$(SHH)CLI rn "TST_Person.name=\"Mike\"" -n "$(PROVIDERNS)" --sum -count 2
	$(SHH)CLI r  "TST_Person.name=\"Mike\"" -n "$(PROVIDERNS)" --sum -count 2
	$(SHH)CLI an "TST_Person.name=\"Mike\"" -n "$(PROVIDERNS)" --sum -count 2
	$(SHH)CLI a  "TST_Person.name=\"Mike\"" -n "$(PROVIDERNS)" --sum -count 2

	@$(ECHO) 5. +++++ TST_Person Testing with  resultClass filters

	$(SHH)CLI rn "TST_Person" -n "$(PROVIDERNS)" -rc TST_Lineage --sum -count 1
	$(SHH)CLI r "TST_Person" -n "$(PROVIDERNS)" -rc TST_Lineage --sum -count 1
	$(SHH)CLI rn "TST_Person.name=\"Mike\"" -n "$(PROVIDERNS)" -rc TST_Lineage --sum -count 2
	$(SHH)CLI r  "TST_Person.name=\"Mike\"" -n "$(PROVIDERNS)" -rc TST_Lineage --sum -count 2

	@$(ECHO) 6. ++++ TST_Person Testing with assocClass filter

	$(SHH)CLI an "TST_Person" -n "$(PROVIDERNS)" -ac  "TST_Lineage" --sum -count 1
	$(SHH)CLI a "TST_Person" -n "$(PROVIDERNS)" -ac "TST_Lineage" --sum -count 1
	$(SHH)CLI an "TST_Person.name=\"Mike\"" -n "$(PROVIDERNS)" -ac "TST_Lineage" --sum -count 2
	$(SHH)CLI a  "TST_Person.name=\"Mike\"" -n "$(PROVIDERNS)" -ac "TST_Lineage" --sum -count 2

	@$(ECHO) 7. ++++ TST_Person Testing with role filter alone

	$(SHH)CLI rn "TST_Person" -n "$(PROVIDERNS)" -r parent --sum -count 4
	$(SHH)CLI r "TST_Person" -n "$(PROVIDERNS)" -r parent --sum -count 4
	$(SHH)CLI rn "TST_Person.name=\"Mike\"" -n "$(PROVIDERNS)" -r parent --sum -count 2
	$(SHH)CLI r  "TST_Person.name=\"Mike\"" -n "$(PROVIDERNS)" -r parent --sum -count 2

	$(SHH)CLI rn "TST_Person" -n "$(PROVIDERNS)" -r child --sum -count 4
	$(SHH)CLI r "TST_Person" -n "$(PROVIDERNS)" -r child --sum -count 4
	$(SHH)CLI rn "TST_Person.name=\"Mike\"" -n "$(PROVIDERNS)" -r child --sum -count 0
	$(SHH)CLI r  "TST_Person.name=\"Mike\"" -n "$(PROVIDERNS)" -r child --sum -count 0

	@$(ECHO) 8. +++++ TEST_Person Test with role and result class

	$(SHH)CLI rn "TST_Person" -n "$(PROVIDERNS)" -rc  TST_Lineage -r parent --sum -count 1
	$(SHH)CLI r "TST_Person" -n "$(PROVIDERNS)" -rc  TST_Lineage -r parent --sum -count 1
	$(SHH)CLI rn "TST_Person.name=\"Mike\"" -n "$(PROVIDERNS)" -rc  TST_Lineage -r parent --sum -count 2
	$(SHH)CLI r  "TST_Person.name=\"Mike\"" -n "$(PROVIDERNS)" -rc  TST_Lineage -r parent --sum -count 2

	@$(ECHO) 9. ++++ TST_Person Test with errors in role and result class

# Bypassing following tests because they generate exception return from provider
# The error is valid because Provider does exception on bad property.
#   But we don't know how to handle that in a script with CLI
#
#	$(SHH)CLI rn "TST_Person" -n "$(PROVIDERNS)" -rc  TST_Lineage -r junk --sum -count 0
#	$(SHH)CLI r "TST_Person" -n "$(PROVIDERNS)" -rc  TST_Lineage -r junk --sum -count 0
#	$(SHH)CLI rn TST_Person.name=\"Mike\" -n "$(PROVIDERNS)" -rc  TST_Lineage -r junk --sum -count 0
#	$(SHH)CLI r  TST_Person.name=\"Mike\" -n "$(PROVIDERNS)" -rc  TST_Lineage -r junk --sum -count 0

#	$(SHH)CLI rn "TST_Person" -n "$(PROVIDERNS)" -rc  "TST_Lineage" -r junk --sum -count 0
#	$(SHH)CLI r "TST_Person" -n "$(PROVIDERNS)" -rc  "TST_Lineage" -r junk --sum -count 0
#	CLI rn "TST_Person.name=\"Mike\"" -n "$(PROVIDERNS)" -rc  "TST_Lineage" -r junk --sum -count 0
#	CLI r  "TST_Person.name=\"Mike\"" -n "$(PROVIDERNS)" -rc  "TST_Lineage" -r junk --sum -count 0

	@$(ECHO) 10. ++++ tests for associations with  result class and result role.

	$(SHH)CLI an  "TST_Person.name=\"Mike\"" -n "$(PROVIDERNS)" -rc  TST_Lineage -r child --sum -count 0
#	$(SHH)CLI an  "TST_Person.name=\"Mike\"" -n "$(PROVIDERNS)" -rc  TST_Lineage -r junk --sum -count 0

#	@$(ECHO) 11. +++++ TST_Person with result Class filters but with bad Class
#Note that we can add more tests here.

	@$(ECHO) 12. +++++ TST_PersonDynamic Family class Operations in  $(PROVIDERNS)

	$(SHH)CLI ei "TST_PersonDynamic" -n "$(PROVIDERNS)" --sum -count 6
	$(SHH)CLI ni "TST_PersonDynamic" -n "$(PROVIDERNS)" --sum -count 6
	$(SHH)CLI rn "TST_PersonDynamic" -n "$(PROVIDERNS)" --sum -count 5
	$(SHH)CLI r  "TST_PersonDynamic" -n "$(PROVIDERNS)" --sum -count 5
	$(SHH)CLI an "TST_PersonDynamic" -n "$(PROVIDERNS)" --sum -count 2
	$(SHH)CLI a  "TST_PersonDynamic" -n "$(PROVIDERNS)" --sum -count 2

	@$(ECHO) 13. +++++ TST_PersonDynamic and Family provider with simple class operations
 
	$(SHH)CLI rn "TST_PersonDynamic.name=\"Father\"" -n "$(PROVIDERNS)" --sum -count 6
	$(SHH)CLI r "TST_PersonDynamic.name=\"Father\"" -n "$(PROVIDERNS)" --sum -count 6
	$(SHH)CLI an "TST_PersonDynamic.name=\"Father\"" -n "$(PROVIDERNS)" --sum -count 6
	$(SHH)CLI a "TST_PersonDynamic.name=\"Father\"" -n "$(PROVIDERNS)" --sum -count 6

	@$(ECHO) "TST_PersonDynamic with bad object name (lower case)"

	$(SHH)CLI rn "TST_PersonDynamic.name=\"father\"" -n "$(PROVIDERNS)" --sum -count 0

	@$(ECHO) 14. ++++ Test TST_PersonDynamic classes with  resultClass/AssocClass options

	$(SHH)CLI rn "TST_PersonDynamic" -n "$(PROVIDERNS)" -rc TST_LineageDynamic --sum -count 1
	$(SHH)CLI r "TST_PersonDynamic" -n "$(PROVIDERNS)" -rc TST_LineageDynamic --sum -count 1
	$(SHH)CLI an "TST_PersonDynamic" -n "$(PROVIDERNS)" -ac TST_LineageDynamic --sum -count 1
	$(SHH)CLI a "TST_PersonDynamic" -n "$(PROVIDERNS)" -ac TST_LineageDynamic --sum -count 1

	@$(ECHO) 15. ++++ TST_PersonDynamic instances with resultClass/AssociationClass options

	$(SHH)CLI rn "TST_PersonDynamic.name=\"Father\"" -n "$(PROVIDERNS)" -rc TST_LineageDynamic --sum -count 3
	$(SHH)CLI r "TST_PersonDynamic.name=\"Father\"" -n "$(PROVIDERNS)" -rc TST_LineageDynamic --sum -count 3
	$(SHH)CLI an "TST_PersonDynamic.name=\"Father\"" -n "$(PROVIDERNS)" -ac TST_LineageDynamic --sum -count 3
	$(SHH)CLI a "TST_PersonDynamic.name=\"Father\"" -n "$(PROVIDERNS)" -ac TST_LineageDynamic --sum -count 3
	$(SHH)CLI a "TST_PersonDynamic.name=\"Father\"" -n "$(PROVIDERNS)" -ac TST_LineageDynamic --sum -count 3

	@$(ECHO) 16. ++++ TST_PersonDynamic association with resultClass role option

	$(SHH)CLI an "TST_PersonDynamic" -n "$(PROVIDERNS)" -rc TST_Lineage --sum -count 0
	$(SHH)CLI an "TST_PersonDynamic" -n "$(PROVIDERNS)" -rc TST_PersonDynamic --sum -count 1
	$(SHH)CLI an "TST_PersonDynamic" -n "$(PROVIDERNS)" -rc TST_Person --sum -count 2

general:

clean:

# END_OF_FILE
