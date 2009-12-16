#//%LICENSE////////////////////////////////////////////////////////////////
#//
#// Licensed to The Open Group (TOG) under one or more contributor license
#// agreements.  Refer to the OpenPegasusNOTICE.txt file distributed with
#// this work for additional information regarding copyright ownership.
#// Each contributor licenses this file to you under the OpenPegasus Open
#// Source License; you may not use this file except in compliance with the
#// License.
#//
#// Permission is hereby granted, free of charge, to any person obtaining a
#// copy of this software and associated documentation files (the "Software"),
#// to deal in the Software without restriction, including without limitation
#// the rights to use, copy, modify, merge, publish, distribute, sublicense,
#// and/or sell copies of the Software, and to permit persons to whom the
#// Software is furnished to do so, subject to the following conditions:
#//
#// The above copyright notice and this permission notice shall be included
#// in all copies or substantial portions of the Software.
#//
#// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
#// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
#// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
#// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
#// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
#// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
#// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#//
#//////////////////////////////////////////////////////////////////////////

# Test cimcli using the cimcli test Provider

ROOT = ../../../../..
DIR = Providers/TestProviders/CLITestProvider/tests

QUOTE=\"

ifeq ($(OS_TYPE),vms)
QUOTE=""
endif

include $(ROOT)/mak/config.mak
include $(ROOT)/mak/configschema.mak

RESULTFILE = $(ROOT)/src/$(DIR)/result
MASTERRESULTFILE = $(ROOT)/src/$(DIR)/result.master
TABLEFORMATFILE = $(ROOT)/src/$(DIR)/tableformat
MASTERTABLEFORMATFILE = $(ROOT)/src/$(DIR)/tableformat.master
HELPRESULTFILE = $(ROOT)/src/$(DIR)/helpresult
MASTERHELPRESULTFILE = $(ROOT)/src/$(DIR)/helpresult.master
NSRESULTFILE = $(ROOT)/src/$(DIR)/namespacetestresult
MASTERNSRESULTFILE = $(ROOT)/src/$(DIR)/namespacetestresult.master

# sampleprovider\Load puts the sample MOF into a separate namespace.
# The following variable defines this namespace.

PROVIDERNS=test/TestProvider
STATICNS=test/static

## Define this namespace because it is based on CIM 2.5 which is
## completely static and which we can use to test some of the
## class and qualifier operations and feel assured that the
## returned information is constant. The static namespace is defined to
## be the current default and so changes with each new pegasus
## default namespace change.
TESTCIMV2=test/cimv2

depend:

sub:

misc:

tests:

messages:

general:


## test the getclass operation.  This serves both as a test of getclass
## and a confirmation that we have not modified the class since the output
## diff will fail if the class is modified
getclass:
	@$(ECHO) 0. +++++ Test For getClass
	@$(ECHO) 0. +++++ Test For getClass  >> $(RESULTFILE)
	cimcli gc -n "$(PROVIDERNS)"  Test_CLITestProviderClass >> $(RESULTFILE)
	cimcli gc -n "$(PROVIDERNS)"  Test_CLITestProviderLinkClass \
	    >> $(RESULTFILE)
	@$(ECHO) 0. +++++ Test getClass

### Test reference parameters
testRefs:
	@$(ECHO) 1. +++++ Test For Reference params
	@$(ECHO) 1. +++++ Test For Reference params  >> $(RESULTFILE)
	##@cimcli im Test_CLITestProviderClass  ReferenceParamTest -n "$(PROVIDERNS)" -ip a=[class.k1=v1,k2=v2,k3=v3]

	##@$(ECHO) 2. +++++ Test String  params  which include square brackets
	##@cimcli im Test_CLITestProviderClass  ReferenceParamTest -n "$(PROVIDERNS)" -ip a="\[string-param\]"


### test get and set property operations
testPropertyOperations:
	@$(ECHO) 2. +++++ Test Getproperty and setProperty
	@$(ECHO) 2. +++++ Test Getproperty and setProperty >> $(RESULTFILE)

#	@cimcli sp  -n "$(PROVIDERNS)" Test_CLITestProviderClass.Id=\"Mike\" Name John
	@cimcli gp  -n "$(PROVIDERNS)" Test_CLITestProviderClass.Id=\"Mike\" Name >> $(RESULTFILE)

	## Test property values for the predefined instance
	@cimcli ni -n "$(PROVIDERNS)" Test_CLITestProviderClass >> $(RESULTFILE)
	@cimcli ei -n "$(PROVIDERNS)" Test_CLITestProviderClass >> $(RESULTFILE)
	@$(ECHO) 2. +++++ Test_CLITestProviderClass Instances

### test create instance operations
testCreateInstanceOperation:
	@$(ECHO) 3. +++++ Test Instances create Instance
	@$(ECHO) 3. +++++ Tests Instances create Instance >> $(RESULTFILE)

	## Test Instance Creation with scalars
	@cimcli ci -n "$(PROVIDERNS)" Test_CLITestProviderClass Id="ScalarTest" \
	scalBool=true scalUint8=12 scalSint8=12 scalUint16=500 \
	scalSint16=500 scalUint32=9999 scalSint32=9999 \
	scalUint64=99999 scalUint64=99999 scalString="testString" \
	>> $(RESULTFILE)

	## Test the creation. Will do error exit if this fails
	@cimcli ti -n "$(PROVIDERNS)" Test_CLITestProviderClass Id="ScalarTest" \
	scalBool=true scalUint8=12 scalSint8=12 scalUint16=500 \
	scalSint16=500 scalUint32=9999 scalSint32=9999 \
	scalUint64=99999 scalUint64=99999 scalString="testString" \
	>> $(RESULTFILE)

	## Test Instance Creation with arrays
	@cimcli ci -n "$(PROVIDERNS)" Test_CLITestProviderClass Id="arrayTest" \
	arrayBool=true,true,true,false,true \
	arrayUint8=12  arraySint8=-127,0,12,127\
	arrayUint32=99,0,31000,63000,4294967295 \
	arraySint32=-32000,0,32000,-2147483648,2147483647 \
	arrayUint64=99,0,31000,63000,18446744073709551615 \
	arraySint64=-32000,0,32000,-9223372036854775808 \
	scalString="testString" \
	scalDateTime=19991224120000.000000+360 >> $(RESULTFILE)

	## run testintance against the instance just created
	@cimcli ti -n "$(PROVIDERNS)" Test_CLITestProviderClass Id="arrayTest" \
	arrayBool=true,true,true,false,true \
	arrayUint8=12  arraySint8=-127,0,12,127\
	arrayUint32=99,0,31000,63000,4294967295 \
	arraySint32=-32000,0,32000,-2147483648,2147483647 \
	arrayUint64=99,0,31000,63000,18446744073709551615 \
	arraySint64=-32000,0,32000,-9223372036854775808 \
	scalString="testString" \
	scalDateTime=19991224120000.000000+360 >> $(RESULTFILE)

        ## Test enumerate of All the instances we created
	@cimcli ni -n "$(PROVIDERNS)" Test_CLITestProviderClass >> $(RESULTFILE)
	@cimcli ei -n "$(PROVIDERNS)" Test_CLITestProviderClass >> $(RESULTFILE)

###
testModifyInstanceOperation:
	@$(ECHO) 4. +++++ Test Instances modify Instance
	@$(ECHO) 4. +++++ Test Instances modify Instance >> $(RESULTFILE)

	@cimcli mi -n "$(PROVIDERNS)" Test_CLITestProviderClass \
	Id="ScalarTest" \
	scalBool=false scalUint8=14 scalSint8=-12 scalUint16=501 \
	scalSint16=501 scalUint32=9998 scalSint32=9999 \
	scalUint64=888 scalUint64=1 >> $(RESULTFILE)   
	     
	@cimcli ti -n "$(PROVIDERNS)" Test_CLITestProviderClass \
	Id="ScalarTest" \
	scalBool=false scalUint8=14 scalSint8=-12 scalUint16=501 \
	scalSint16=501 scalUint32=9998 scalSint32=9999 \
	scalUint64=888 scalUint64=1 scalString=testString >> $(RESULTFILE)

##### NOTE: This test reflects a possible error in Pegasus today.  The
#####       key property should not be required in the input.
	@cimcli mi -n "$(PROVIDERNS)" \
	Test_CLITestProviderClass.Id=\"ScalarTest\" \
	Id=ScalarTest \
	scalBool=true scalUint8=15 scalSint8=-13 scalUint16=502 \
	scalSint16=502 scalUint32=9997 scalSint32=9998 \
	scalUint64=888 scalUint64=1 >> $(RESULTFILE)

	@cimcli ti -n "$(PROVIDERNS)" Test_CLITestProviderClass \
	Id=ScalarTest \
	scalBool=true scalUint8=15 scalSint8=-13 scalUint16=502 \
	scalSint16=502 scalUint32=9997 scalSint32=9998 \
	scalUint64=888 scalUint64=1 >> $(RESULTFILE)

	@$(ECHO) +++++ Test modify instance Passed.


### test Delete instance. Assumes instances created with testCreateInstance
testDeleteInstanceOperation:	
	@$(ECHO) 5. +++++ Test delete instance
	@$(ECHO) 5. +++++ Test delete instance >> $(RESULTFILE)

	## test delete with class name=value input
	@cimcli di -n "$(PROVIDERNS)" \
	    Test_CLITestProviderClass.Id=\"ScalarTest\" >> $(RESULTFILE)
	@cimcli di -n "$(PROVIDERNS)" \
	    Test_CLITestProviderClass Id=arrayTest >> $(RESULTFILE)

        ## test with CIMObjectPath form of input
	@cimcli ci -n "$(PROVIDERNS)" Test_CLITestProviderClass \
	Id="ScalarTest" \
	scalBool=true  \
	scalUint8=12 scalSint8=12 \
	scalUint16=500 scalSint16=500 \
	scalUint32=9999 scalSint32=9999 \
	scalUint64=99999 scalUint64=99999 \
	scalString="testString" \
	>> $(RESULTFILE)

	## Test the creation. Will do error exit if this fails
	@cimcli ti -n "$(PROVIDERNS)" Test_CLITestProviderClass \
	    Id="ScalarTest" \
	scalBool=true scalUint8=12 scalSint8=12 scalUint16=500 \
	scalSint16=500 scalUint32=9999 scalSint32=9999 \
	scalUint64=99999 scalUint64=99999 scalString="testString" \
	>> $(RESULTFILE)

	@cimcli di -n "$(PROVIDERNS)" \
	    Test_CLITestProviderClass Id=ScalarTest >> $(RESULTFILE)

	@$(ECHO) +++++ Test delete instance Passed.

### test invoke method with a variety of parameter  data types
testInvokeMethod:
	@$(ECHO) 6. +++++ Test invokeMethod
	@$(ECHO) 6. +++++ Test invokeMethod >> $(RESULTFILE)

	## test using inOutParameter method that returns all
	## input parameters as corresponding output parameters
	@cimcli im Test_CLITestProviderClass -n "$(PROVIDERNS)" \
	        InOutParamTest \
		scalBool=true \
		arrayBool=true,false,true \
		scalUint32=99 \
		arrayUint32=99,0,31000,63000 \
		scalSint32=-99 \
		arraySint32=-32000,0,32000 \
		scalString=ThisIsMyTest \
		arrayString="one",two,"three","multiword string" \
		scalDateTime=19991224120000.000000+360 \
		arrayDateTime=19991224120000.000000+360,19991224120000.000000+360 \
		>>$(RESULTFILE)
	@$(ECHO) +++++ Test invoke method Passed.

### test cimcli output with the table output format
testTableOutputFormat:
	@$(ECHO) 7. +++++ Test table output format
	@$(ECHO) 7. +++++ Test table output format >> $(RESULTFILE)

	@cimcli ci -n "$(PROVIDERNS)" Test_CLITestProviderClass \
	    Id=Norman Name=Susan
	@cimcli ci -n "$(PROVIDERNS)" Test_CLITestProviderClass \
	    Id=Thomas Name=Peter
	cimcli ei -n "$(PROVIDERNS)" \
	    -o table Test_CLITestProviderClass >> $(TABLEFORMATFILE)

	@cimcli di -n "$(PROVIDERNS)" Test_CLITestProviderClass.Id=\"Norman\"
	@cimcli di -n "$(PROVIDERNS)" Test_CLITestProviderClass.Id=\"Thomas\"
	@$(ECHO) +++++ Test table output format Passed.

###
###     For now we only test getQualifier.  Enumerate can have too many
###     surprises and set not implemented.  
testQualifierDecls:
	@$(ECHO) 8. +++++ Test QualifierDecl Operations
	@$(ECHO) 8. +++++ Test QualifierDecl Operations >> $(RESULTFILE)

	@cimcli gq -n "$(TESTCIMV2)" Association >>$(RESULTFILE)
	@$(ECHO) +++++ Test QualifierDecls Passed.

###
###     Test the class operations.  Note that we may have issues with the
###     enumerates and ordering. Also we use the static namespace to 
###     assure that the classes do not change
testClassOperations:
	@$(ECHO) 9. +++++ getClass Operation
	@$(ECHO) 9. +++++ getClass Operation >> $(RESULTFILE)

	@cimcli nc -n $(TESTCIMV2) --sum -count 64 >>$(RESULTFILE)
	@cimcli nc -n $(TESTCIMV2) >>$(RESULTFILE)
	@cimcli gc CIM_ManagedElement -n $(TESTCIMV2) >>$(RESULTFILE)
	@cimcli gc CIM_ManagedElement -n $(TESTCIMV2) -o xml >>$(RESULTFILE)
	@$(ECHO) +++++ Test ClassOperations Passed.

testPropertyListOption:
	@$(ECHO) 10. +++++ testPropertyListOptions
	@$(ECHO) 10. +++++ testPropertyListOptions >> $(RESULTFILE)

	@cimcli gc CIM_ManagedElement -n $(STATICNS) -pl Caption \
	    >>$(RESULTFILE)
	@cimcli gc CIM_ManagedElement -n $(STATICNS) \
	 -pl Caption,Description >>$(RESULTFILE)
	@cimcli gc CIM_ManagedElement -n $(STATICNS) \
	 -pl Caption,Description,ElementName >>$(RESULTFILE)
	@cimcli gc CIM_ManagedElement -n $(STATICNS) -pl "" >>$(RESULTFILE)

	@cimcli ei -n "$(PROVIDERNS)" Test_CLITestProviderClass \
	-pl scalBool,arrayUint64 >>$(RESULTFILE)
	@cimcli ei -n "$(PROVIDERNS)" Test_CLITestProviderClass \
	-pl "" >>$(RESULTFILE)

	@cimcli gi -n "$(PROVIDERNS)" Test_CLITestProviderClass.Id=\"Mike\" \
	-pl scalBool,arrayUint64,scalString  >>$(RESULTFILE)
	@$(ECHO) +++++ Test testPropertyListOption Passed.

# Test the different formats for input of getinstance operation
testGetInstanceOperation:
	@$(ECHO) 12. +++++ test getInstance Operation
	@$(ECHO) 12. +++++ test getInstance Operation >> $(RESULTFILE)

	@cimcli gi Test_CLITestProviderClass -n "$(PROVIDERNS)" Id=Mike \
	  >>$(RESULTFILE)

	@cimcli gi Test_CLITestProviderClass -n "$(PROVIDERNS)" Id=Mike \
	  -pl Name,scalBool >>$(RESULTFILE)

	@cimcli gi Test_CLITestProviderClass.Id=\"Mike\" -n "$(PROVIDERNS)" \
	  -pl Name,scalBool >>$(RESULTFILE)
	   
	@$(ECHO) +++++ Test getInstance Passed.

# Test the testInstance Operation
# Tests for positive (No error responses).
testTestInstanceOperation:
	@$(ECHO) 13. +++++ test TestInstance Operations
	@$(ECHO) 13. +++++ test TestInstance Operations >> $(RESULTFILE)

	@cimcli ti -n "$(PROVIDERNS)" Test_CLITestProviderClass Id=Mike \
	    Name=Bob  >>$(RESULTFILE)
	@cimcli ti -n "$(PROVIDERNS)" Test_CLITestProviderClass.Id=\"Mike\" \
	    Name=Bob  >>$(RESULTFILE)
	@cimcli ti -n "$(PROVIDERNS)" Test_CLITestProviderClass Id=Mike \
	    scalSint32=100  >>$(RESULTFILE)
	@cimcli ti -n "$(PROVIDERNS)" Test_CLITestProviderClass Id=Mike \
	    scalBool=true \
	    scalSint32=100  >>$(RESULTFILE)
	@cimcli ti -n "$(PROVIDERNS)" Test_CLITestProviderClass Id=Mike \
	    arrayUint32=4,128,240  >>$(RESULTFILE)
	@cimcli ti -n "$(PROVIDERNS)" Test_CLITestProviderClass Id=Mike \
	    arrayString="First","Second","Third"  >>$(RESULTFILE)
	@cimcli ti -n "$(PROVIDERNS)" Test_CLITestProviderClass Id=Mike \
	    scalBool=true \
	    scalSint32=100 \
	    arrayUint32=4,128,240 \
	    arrayString="First","Second","Third"  >>$(RESULTFILE)

	@$(ECHO) +++++ Test testInstance Passed.

## test integer input and test with decimal, hex, octal and binary integers
testIntegerVariations:
	## Test variations on integer values
	@$(ECHO) 14. +++++ test Integer Variations
	@$(ECHO) 14. +++++ test Integer Variations >> $(RESULTFILE)

	
	@cimcli ci -n "$(PROVIDERNS)" Test_CLITestProviderClass \
	    Id=integerVariationTest \
	    scalBool=true \
	    scalUint8=12 \
	    scalUint16=100 \
	    scalUint32=9999 \
	    scalUint64=99999 \
	    arrayUint32=99,0,31000,63000,4294967295 >> $(RESULTFILE)

	## display the created instance
	@cimcli gi -n "$(PROVIDERNS)" Test_CLITestProviderClass \
	   Id=integerVariationTest \
            >> $(RESULTFILE)

        ## test values input
	@cimcli ti -n "$(PROVIDERNS)" Test_CLITestProviderClass \
	    Id=integerVariationTest \
	    scalUint8=12 \
	    scalUint16=100 \
	    scalUint32=9999 \
	    scalUint64=99999 \
	    arrayUint32=99,0,31000,63000,4294967295 >> $(RESULTFILE)

        ## test Hex
	@cimcli ti -n "$(PROVIDERNS)" Test_CLITestProviderClass \
	    Id=integerVariationTest \
	    scalBool=true \
	    scalUint8=0xC
	    scalUint16=0X64 \
	    scalUint32=0X270F \
	    scalUint64=0X1869F \
	    arrayUint32=0X63,0,0X7918,0XF618,0X19999999 >> $(RESULTFILE)

	## test Octal
	@cimcli ti -n "$(PROVIDERNS)" Test_CLITestProviderClass \
	    Id=integerVariationTest \
	    scalBool=true \
	    scalUint8=014
	    scalUint16=0144 \
	    scalUint32=023417 \
	    scalUint64=0303237 >> $(RESULTFILE)

	## test Binary
	@cimcli ti -n "$(PROVIDERNS)" Test_CLITestProviderClass \
	    Id=integerVariationTest \
	    scalBool=true \
	    scalUint8=1100b
	    scalUint16=1100100b \
	    scalUint32=10011100001111b >> $(RESULTFILE)

	@cimcli di -n "$(PROVIDERNS)" Test_CLITestProviderClass \
	    Id=integerVariationTest >> $(RESULTFILE)

	@$(ECHO) +++++ Test integerVariations Passed.

## test the reference and reference name operations
referencetests:
	@$(ECHO) 15. +++++ referencetests
	@$(ECHO) 15. +++++ referencetests >> $(RESULTFILE)
	cimcli r Test_CLITestProviderClass -n "$(PROVIDERNS)" \
	    >> $(RESULTFILE)
	cimcli rn Test_CLITestProviderClass -n "$(PROVIDERNS)" \
	    >> $(RESULTFILE)
	cimcli r -n "$(PROVIDERNS)" Test_CLITestProviderClass.Id=\"Mike\" \
	    >> $(RESULTFILE)
	cimcli r -n "$(PROVIDERNS)" Test_CLITestProviderClass.Id=\"Mike\" \
		-r parent  >> $(RESULTFILE)
	cimcli rn -n "$(PROVIDERNS)" Test_CLITestProviderClass.Id=\"Mike\" \
	    >> $(RESULTFILE)
## This test confirms that specific input parameters are correctly passed
## to the provider since the test provider returns this information
## in a specific property.
	cimcli r -n "$(PROVIDERNS)" Test_CLITestProviderClass.Id=\"Mike\" \
	    -r child -rc Test_CLITestProviderLinkClass \
	    -pl requestInputParameters >> $(RESULTFILE)
	@$(ECHO) +++++ Test referencetests Passed.

## Test operation of the association and association name operations
associationtests:
	@$(ECHO) 16. +++++ associationtests
	@$(ECHO) 16. +++++ associationtests >> $(RESULTFILE)
	cimcli a Test_CLITestProviderClass -n "$(PROVIDERNS)" >> $(RESULTFILE)
	cimcli an Test_CLITestProviderClass -n "$(PROVIDERNS)" >> $(RESULTFILE) 
	cimcli a -n "$(PROVIDERNS)" Test_CLITestProviderClass.Id=\"Mike\" \
	    >> $(RESULTFILE)
	cimcli an -n "$(PROVIDERNS)" Test_CLITestProviderClass.Id=\"Mike\" \
	    >> $(RESULTFILE)
	cimcli a -n "$(PROVIDERNS)" Test_CLITestProviderClass.Id=\"Mike\" \
	    -pl "" >> $(RESULTFILE) 
###     This test returns property with the input parameters
	cimcli a -n "$(PROVIDERNS)" Test_CLITestProviderClass.Id=\"Mike\" \
	    -ac Test_CLITestProviderLinkClass -rc Test_CLITestProviderClass \
	    -r child -rr parent -pl requestInputParameters \
	    >> $(RESULTFILE)
	@$(ECHO) +++++ Test associationtests Passed.


## test cimcli response to help option requests
helpoutputtests:
	@$(ECHO) 17. +++++ test Help Output
	@$(ECHO) 17. +++++ test Help Output >> $(HELPRESULTFILE)

	@cimcli --help >> $(HELPRESULTFILE)
	@cimcli -hc  >> $(HELPRESULTFILE)
	@cimcli -ho >> $(HELPRESULTFILE)
	@cimcli -h mi  >> $(HELPRESULTFILE)

	@$(ECHO) +++++ Test Help Output Passed.

##
##      Test the parameter options for property definitions that
##      produce null or default property values
##
nullvaluetests:
	@$(ECHO) 18. +++++ nullvaluetests
	@$(ECHO) 18. +++++ nullvaluetests >> $(RESULTFILE)
	@cimcli ci -n "$(PROVIDERNS)" Test_CLITestProviderClass \
	Id="NullValueTest" \
	scalBool= defaultBool= \
	scalUint32= defaultUint32= \
	scalString= defaultString=  >> $(RESULTFILE)

	@cimcli gi -n "$(PROVIDERNS)" Test_CLITestProviderClass \
	Id="NullValueTest"  >> $(RESULTFILE)
		
	@cimcli ti -n "$(PROVIDERNS)" Test_CLITestProviderClass \
	Id="NullValueTest"
	scalBool= defaultBool= \
	scalUint32= defaultUint32= \
	scalString= defaultString=  >> $(RESULTFILE)

	@cimcli di -n "$(PROVIDERNS)" Test_CLITestProviderClass \
	Id="NullValueTest"  >> $(RESULTFILE)

	@cimcli ci -n "$(PROVIDERNS)" Test_CLITestProviderClass \
	    Id=EmptyStringTest \
	    scalUint32= \
	    scalUint64=99999 \
	    arrayUint32=99,0,31000,63000,4294967295 \
	    scalString= \
	    defaultString!  >> $(RESULTFILE)

	@cimcli gi -n "$(PROVIDERNS)" Test_CLITestProviderClass \
	Id="EmptyStringTest"  >> $(RESULTFILE)

	@cimcli ti -n "$(PROVIDERNS)" Test_CLITestProviderClass \
	    Id=EmptyStringTest \
	    scalUint32= \
	    scalUint64=99999 \
	    arrayUint32=99,0,31000,63000,4294967295 \
	    scalString= \
	    defaultString!  >> $(RESULTFILE)

	cimcli di -n "$(PROVIDERNS)" Test_CLITestProviderClass \
	Id="EmptyStringTest"  >> $(RESULTFILE)

	@$(ECHO) +++++ Test nullvaluetests Passed.
##
##	test the ns operation.  Note that this test is subject to knowledge
##	of the number of namespaces and their names in the target system
##	We assume that this is generally a constant for the pegasus nightly
##	test environment.
##
namespacetests:
	@$(ECHO) 19. +++++ namespacetests
	@$(ECHO) 19. +++++ namespacetests >> $(NSRESULTFILE)
	@cimcli ns >> $(NSRESULTFILE)
	@cimcli ns --sum >> $(NSRESULTFILE)
	@cimcli ns --sum -count 15

	@$(ECHO) +++++ Test namespacetests Passed.

## execute the set of targets defined below
poststarttests:
	## remove existing master files
	@$(RM) $(RESULTFILE)
	@$(RM) $(TABLEFORMATFILE)
	@$(RM) $(HELPRESULTFILE)
	@$(RM) $(NSRESULTFILE)
	
	## execute the list of tests
	$(MAKE) getclass
	$(MAKE) testQualifierDecls
	$(MAKE) testClassOperations
	$(MAKE) testRefs
	$(MAKE) testPropertyOperations
	$(MAKE) testCreateInstanceOperation
	$(MAKE) testModifyInstanceOperation
	$(MAKE) testDeleteInstanceOperation
	$(MAKE) testInvokeMethod
	$(MAKE) testTableOutputFormat
	$(MAKE) testPropertyListOption
	$(MAKE) testGetInstanceOperation
	$(MAKE) testTestInstanceOperation
	$(MAKE) testIntegerVariations
	$(MAKE) referencetests
	$(MAKE) associationtests
	$(MAKE) helpoutputtests
	$(MAKE) nullvaluetests
	$(MAKE) namespacetests

	@$(ECHO) +++++ Compare Result files
	@$(DIFF) $(MASTERRESULTFILE) $(RESULTFILE)
	@$(DIFF) $(MASTERTABLEFORMATFILE) $(TABLEFORMATFILE)
	@$(DIFF) $(MASTERHELPRESULTFILE) $(HELPRESULTFILE)
	## this is separate because we cannot depend on the results
	## from this test (i.e. exact number of namespaces
	## so may want to remove the comparison
	-@$(DIFF) $(MASTERNSRESULTFILE) $(NSRESULTFILE)

	@$(ECHO) +++++ Remove Result files since test successful
	@$(RM) $(RESULTFILE)
	@$(RM) $(TABLEFORMATFILE)
	@$(RM) $(HELPRESULTFILE)
	@$(RM) $(NSRESULTFILE)

	@$(ECHO) +++++ All cimcli tests Passed.

##
## Cleans up an created/modified/or deleted instances that may exist
## in the server (i.e. the provider) and removes the result files.
## Note that the normal path also removes result files so that the
## only reason for this is to redo a failed test before the provider is
## reinitialized (server restarted or provider reloaded)
clean:
	## remove any existing instances
	-@cimcli di -n "$(PROVIDERNS)" \
	    Test_CLITestProviderClass.Id=\"Norman\"
	-@cimcli di -n "$(PROVIDERNS)" \
	    Test_CLITestProviderClass.Id=\"Thomas\"
	-@cimcli di -n "$(PROVIDERNS)" \
	    Test_CLITestProviderClass.Id=\"ScalarTest\"
	-@cimcli di -n "$(PROVIDERNS)" Test_CLITestProviderClass Id=arrayTest
	-@cimcli di -n "$(PROVIDERNS)" Test_CLITestProviderClass Id=stringTest
	-@cimcli di -n "$(PROVIDERNS)" Test_CLITestProviderClass \
	    Id=integerVariationTest
	-@cimcli di -n "$(PROVIDERNS)" Test_CLITestProviderClass \
	    Id="NullValueTest"
	-@cimcli di -n "$(PROVIDERNS)" Test_CLITestProviderClass \
	    Id="EmptyStringTest"

	## clean out the result files
	@$(RM) $(RESULTFILE)
	@$(RM) $(TABLEFORMATFILE)
	@$(RM) $(HELPRESULTFILE)
	@$(RM) $(NSRESULTFILE)




giall:

	cimcli gi -n "$(PROVIDERNS)" Test_CLITestProviderClass

diall:

	cimcli gi -n "$(PROVIDERNS)" Test_CLITestProviderClass

## This is useful display during development. Displays classes
## used in this test
getclasslocal:
	cimcli gc -n "$(PROVIDERNS)"  Test_CLITestProviderClass
	cimcli gc -n "$(PROVIDERNS)"  Test_CLITestProviderLinkClass

# END_OF_FILE

