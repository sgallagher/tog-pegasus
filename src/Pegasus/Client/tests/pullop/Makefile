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
ROOT = ../../../../..
DIR = Pegasus/Client/tests/pullop
include $(ROOT)/mak/config.mak
include ../libraries.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

PROGRAM = pullop

SOURCES = pullop.cpp

include $(ROOT)/mak/program.mak

##############################
## Test cmpi provider logistics
CMPICL = CMPISample_InstanceProviderClass
CMPIASSOCOBJ =  CMPI_TEST_Person.name=\"Melvin\"
CMPIASSOCOBJ2 = CMPI_TEST_Vehicle.vehiclename="\Ferrari\"

## ResponseStressTest provider class and namespace
NStressCxx = test/testProvider
CStressCxx = TST_ResponseStressTestCxx

## Simple C++ Provider classes and namespaces
CxxNS = root/SampleProvider
CxxCL = Sample_InstanceProviderClass

## C++ association Provider Object and Namespace
CxxAssocOB = Test_CLITestProviderClass.Id=\"Mike\"
CxxAssocNS = test/TestProvider

HOSTID = -H localhost
CIMCLIHOSTID = -l localhost

## Unit test. This is not a compelete test since this is a test program
## Simply assures that we do not crash when help executed.
tests:
	pullop -h
	$(PROGRAM) --help

## KS_TODO - Redo this list to actually put in the pullop code for each
## Will make it more understandable and also can work with options easier.
errortests:
	make filterParamError
	make pullClassParamError
	make continueOnErrorError
	make invalidAssocObjectPath
	make maxPullObjectCountError

poststarttests:
	@$(ECHO)  Enumerate and EnumerateNames tests go to Completion
	make smallTestEnumerateInstances
	make smallTestEnumerateInstanceNames
	make peManagedElement
	make penManagedElement
	make cmpiEnumerate1
	make cmpiEnumerateNames1
	make enumerateMaxObjectSizeVariations
	make enumeratePathsMaxObjectSizeVariations
	make enumerateEmptyResponse
	make stresse1
	make stresse2
	make stressen1
	make stressen2

	@$(ECHO)  Associator and Reference tests got to completion
	make SimpleRefAndAssoc
	make SimpleAssociatorTest1
	make SimpleAssociatorTest2
	make SimpleAssociatorTest3
	make pcmpir1
	make pcmpirn1
	make pcmpian1
	make pcmpia1

	@$(ECHO)  Test close in process of delivery
	make testenumclose
	make testenumpathsclose
##
##       @$(ECHO)  Test timeout in process of delivery
##       make testtimeout
##
	@$(ECHO) Error tests. Tests simple parameter errors.
	make errortests

	@$(ECHO) +++++ All pullop Pull Operation tests Passed.

## Enumerate a class that is a C++ Provider. Very small enumerate
## Normally returns about 3 instances
smallTestEnumerateInstances:
## Return all with initial request
	$(PROGRAM) e -c $(CxxCL) -M 5 -N 5  -n $(CxxNS) -C -T $(HOSTID)
	$(PROGRAM) e -c $(CxxCL) -M 5 -N 5  -n $(CxxNS) -C -T $(HOSTID) -P Message
	$(PROGRAM) e -c $(CxxCL) -M 5 -N 5  -n $(CxxNS) -C -T $(HOSTID) -P ""
## Return with probably one pull
	$(PROGRAM) e -c $(CxxCL) -M 1 -N 5 -n $(CxxNS) -C -T $(HOSTID)
	$(PROGRAM) e -c $(CxxCL) -M 1 -N 5 -n $(CxxNS) -C -T $(HOSTID) -P Message
	$(PROGRAM) e -c $(CxxCL) -M 1 -N 5 -n $(CxxNS) -C -T $(HOSTID) -P ""

## test variations of open max object count and pull max object count
enumerateMaxObjectSizeVariations:
	$(PROGRAM) e -c $(CxxCL) -M 1 -N 1 -n $(CxxNS) -C -T $(HOSTID)
	$(PROGRAM) e -c $(CxxCL) -M 2 -N 2 -n $(CxxNS) -C -T $(HOSTID)
	$(PROGRAM) e -c $(CxxCL) -M 3 -N 3 -n $(CxxNS) -C -T $(HOSTID)
	$(PROGRAM) e -c $(CxxCL) -M 0 -N 1 -n $(CxxNS) -C -T $(HOSTID)
	$(PROGRAM) e -c $(CxxCL) -M 0 -N 3 -n $(CxxNS) -C -T $(HOSTID)
	$(PROGRAM) e -c $(CxxCL) -M 0 -N 100 -n $(CxxNS) -C -T $(HOSTID)
	$(PROGRAM) e -c $(CxxCL) -M 100 -N 10 -n $(CxxNS) -C -T $(HOSTID)

enumeratePathsMaxObjectSizeVariations:
	$(PROGRAM) en -c $(CxxCL) -M 1 -N 1 -n $(CxxNS) -C -T $(HOSTID)
	$(PROGRAM) en -c $(CxxCL) -M 2 -N 2 -n $(CxxNS) -C -T $(HOSTID)
	$(PROGRAM) en -c $(CxxCL) -M 3 -N 3 -n $(CxxNS) -C -T $(HOSTID)
	$(PROGRAM) en -c $(CxxCL) -M 0 -N 1 -n $(CxxNS) -C -T $(HOSTID)
	$(PROGRAM) en -c $(CxxCL) -M 0 -N 3 -n $(CxxNS) -C -T $(HOSTID)
	$(PROGRAM) en -c $(CxxCL) -M 0 -N 100 -n $(CxxNS) -C -T $(HOSTID)
	$(PROGRAM) en -c $(CxxCL) -M 100 -N 10 -n $(CxxNS) -C -T $(HOSTID)

## enumerate instance names of a class in a C++ provider
smallTestEnumerateInstanceNames:
	$(PROGRAM) en -c $(CxxCL) -M 1 -N 1  -n $(CxxNS) -C $(HOSTID)
	$(PROGRAM) en -c $(CxxCL) -M 5 -N 5  -n $(CxxNS) -C -T $(HOSTID)
	$(PROGRAM) en -c $(CxxCL) -M 0 -N 1  -n $(CxxNS) -C -T $(HOSTID)

## enumerate from cmpi providers. There are 3 tests here to include
## property list variations
cmpiEnumerate1:
	$(PROGRAM) e -c $(CMPICL) -M 1 -N 1  -n root/SampleProvider -C -T $(HOSTID)
	$(PROGRAM) e -c $(CMPICL) -M 1 -N 1  -n root/SampleProvider -C -T $(HOSTID) -P Message
	$(PROGRAM) e -c $(CMPICL) -M 1 -N 1  -n root/SampleProvider -C -T $(HOSTID) -P ""

## enumerate from cmpi providers
cmpiEnumerateNames1:
	$(PROGRAM) en -c $(CMPICL) -M 1 -N 1 -n root/SampleProvider -C -T $(HOSTID)

## Simple pull ref and assoc operations.
SimpleRefAndAssoc:
	$(PROGRAM) r -c $(CxxAssocOB) -n $(CxxAssocNS) -C $(HOSTID)
	$(PROGRAM) rn -c $(CxxAssocOB) -n $(CxxAssocNS) -C $(HOSTID)
	$(PROGRAM) a -c $(CxxAssocOB) -n $(CxxAssocNS) -C $(HOSTID)
	$(PROGRAM) an -c $(CxxAssocOB) -n $(CxxAssocNS) -C $(HOSTID)

## returns a single association object and asks for one object per request
## compares results with assoc request so counts should match.  This
SimpleAssociatorTest1:
## this test confirms that we are getting one object
	cimcli a $(CxxAssocOB) -n $(CxxAssocNS) --sum -count 1
	$(PROGRAM) a -c $(CxxAssocOB)  -M 1 -N 1 -C -T -n $(CxxAssocNS) -C $(HOSTID)
	$(PROGRAM) an -c $(CxxAssocOB) -M 1 -N 1 -C -T -n $(CxxAssocNS) -C $(HOSTID)
	$(PROGRAM) r -c $(CxxAssocOB)  -M 1 -N 1 -C -T -n $(CxxAssocNS) -C $(HOSTID)
	$(PROGRAM) rn -c $(CxxAssocOB) -M 1 -N 1 -C -T -n $(CxxAssocNS) -C $(HOSTID)

## Request class that returns a single object but ask for more than one in
## response

SimpleAssociatorTest2: pa2 pan2 pr2 prn2
pa2:
	$(PROGRAM) a -c Test_CLITestProviderClass.Id=\"Mike\" -C -n test/TestProvider -C $(HOSTID)

pan2:
	$(PROGRAM) an -c Test_CLITestProviderClass.Id=\"Mike\" -C -n test/TestProvider -C $(HOSTID)

pr2:
	$(PROGRAM) rn -c Test_CLITestProviderClass.Id=\"Mike\" -C -n test/TestProvider -C $(HOSTID)

prn2:
	$(PROGRAM) rn -c Test_CLITestProviderClass.Id=\"Mike\" -C -n test/TestProvider -C $(HOSTID)


## Request class that returns objects with some in repository
SimpleAssociatorTest3: pa3 pan3 pr3 prn3
pa3:
	$(PROGRAM) a -N 1 -N 1 -c Person.name=\"Mike\" -n root/SampleProvider -C $(HOSTID)
pan3:
	$(PROGRAM) an -N 1 -N 1 -c Person.name=\"Mike\" -n root/SampleProvider -C $(HOSTID)
pr3:
	$(PROGRAM) a -N 1 -N 1 -c Person.name=\"Mike\" -n root/SampleProvider -C $(HOSTID)
prn3:
	$(PROGRAM) an -N 1 -N 1 -c Person.name=\"Mike\" -n root/SampleProvider -C $(HOSTID)

######################################################################
peManagedElement: peManagedElementcimv2

peManagedElementcimv2:
	$(PROGRAM) e -c CIM_ManagedElement -M 100 -N 100 -v 5 -n root/cimv2 $(HOSTID)

## generates error because of qualifier data and classorigin data from PG_Namespace
peManagedElementInterop:
	$(PROGRAM) e -c CIM_ManagedElement -M 9 -N 1 -C -n root/PG_InterOp $(HOSTID)

penManagedElement:
	$(PROGRAM) en -c CIM_ManagedElement -M 50 -N 9 -C -n root/cimv2 $(HOSTID)

#########################################################
penManagedElement: penManagedElementcimv2

penManagedElementcimv2:
	$(PROGRAM) en -c CIM_ManagedElement -M 9 -N 4 -C $(HOSTID)

penManagedElementInterop:
	$(PROGRAM) en -c CIM_ManagedElement -M 9 -N 1 -C -n root/PG_InterOp $(HOSTID)

## Test Empty response.
enumerateEmptyResponse:
	$(PROGRAM) e -c CIM_ConcreteIdentity -n root/cimv2 $(HOSTID)
	$(PROGRAM) en -c CIM_ConcreteIdentity -n root/cimv2 $(HOSTID)

## test with large scale response provider
## Execute the large scale test with just one property in response.
## issue because there is one property that may differ between instances
## the interval property that defines intervals between the delivery
## of subsequent instances to the CIMOM and therefore is not constant.
## Included list of properties to return to make this test valid.
## CIMCLi is called before each test to set the response size and number
## of responses requested
## Calling the invoke method before starting each test forces
## the provider to be loaded which eliminates startup differences.
## stresse1 object size 10,000, response coutn 10,000
## Do not include interval as a property in the response because it
## can differ between requests.
## Set large client timeout here because with the stress test sizes, they
## fail in valgrind with smaller client timeout.
stresse1:
	cimcli im $(CStressCxx) set -n $(NStressCxx) Size=10000 ResponseCount=10000 $(CIMCLIHOSTID)
	$(PROGRAM) e -c $(CStressCxx) -n $(NStressCxx) -C \
	    -y 120 \
	    -P Pattern -P s1 -P Id -P SequenceNumber \
	    -T -v 3 $(HOSTID)

## test with object size 1000 and 100,000 returned objects
stresse2:
	cimcli im $(CStressCxx) set -n $(NStressCxx) Size=1000 ResponseCount=100000 $(CIMCLIHOSTID)
	$(PROGRAM) e -c $(CStressCxx) -n $(NStressCxx) -C -y 120\
	    -P Pattern -P s1 -P Id -P SequenceNumber \
	    -T -v 3 $(HOSTID)

stressen1:
	cimcli im $(CStressCxx) set -n $(NStressCxx) Size=10000 ResponseCount=10000 $(CIMCLIHOSTID)
	$(PROGRAM) en -c $(CStressCxx) -n $(NStressCxx) -C -y 120 -T -v 3 $(HOSTID)

stressen2:
	cimcli im $(CStressCxx) set -n $(NStressCxx) Size=1000 ResponseCount=100000 $(CIMCLIHOSTID)
	$(PROGRAM) en -c $(CStressCxx) -n $(NStressCxx) -C -y 120 -T -v 3 $(HOSTID)

## test of variation of response  max size form very small to large
stressVariation:
	cimcli im $(CStressCxx) set -n $(NStressCxx) Size=1000 ResponseCount=10000 $(CIMCLIHOSTID)
	$(PROGRAM) e -c $(CStressCxx) -n $(NStressCxx) -M 10 -N 10 -C -T -P Pattern -P s1 -P Id -P SequenceNumber -T -v 3 $(HOSTID)
	$(PROGRAM) e -c $(CStressCxx) -n $(NStressCxx) -M 100 -N 100 -C -T -P Pattern -P s1 -P Id -P SequenceNumber -T -v 3 $(HOSTID)
	$(PROGRAM) e -c $(CStressCxx) -n $(NStressCxx) -M 1000 -N 1000 -C -T -P Pattern -P s1 -P Id -P SequenceNumber -T -v 3 $(HOSTID)

### Error Tests
## These tests actually return exit 0 because the -R option reverses the
## exit codes so the following should actually return positive exits for
## make
##
pullClassParamError:
	$(PROGRAM) e -c blah -R $(HOSTID)
	$(PROGRAM) en -c blah -R $(HOSTID)

## Generates an error because reqested object count too large. Currently
## maximum is fixed at 1000
## FUTURE: When we implement runtime setting of maximum, we may have an
## issue with this test.
maxPullObjectCountError:
	$(PROGRAM) e -c $(CStressCxx) -n $(NStressCxx) -M 9999 -N 9999 -C -R -T -v 3 $(HOSTID)

# the following tests should pass Makefile exit error tests.
filterParamError:
	$(PROGRAM) e Test_Person -f abc -R $(HOSTID)
	$(PROGRAM) e Test_Person -l WQL -R $(HOSTID)
	$(PROGRAM) en Test_Person -f abc -R $(HOSTID)
	$(PROGRAM) en Test_Person -l WQL -R $(HOSTID)
	$(PROGRAM) r Test_Person -f abc -R $(HOSTID)
	$(PROGRAM) r Test_Person -l WQL -R $(HOSTID)
	$(PROGRAM) rn Test_Person -f abc -R $(HOSTID)
	$(PROGRAM) rn Test_Person -l WQL -R $(HOSTID)
	$(PROGRAM) a Test_Person -f abc -R $(HOSTID)
	$(PROGRAM) a Test_Person -l WQL -R $(HOSTID)
	$(PROGRAM) an Test_Person -f abc -R $(HOSTID)
	$(PROGRAM) an Test_Person -l WQL -R $(HOSTID)

## test continueOnError Parameter.  Should return error in all cases since
## continueOnError not supported
continueOnErrorError:
	$(PROGRAM) e Test_Person  -R -x $(HOSTID)
	$(PROGRAM) en Test_Person  -R -x $(HOSTID)
	$(PROGRAM) r -c Test_CLITestProviderClass.Id=\"Mike\" -n test/TestProvider -R -x $(HOSTID)
	$(PROGRAM) rn -c Test_CLITestProviderClass.Id=\"Mike\" -n test/TestProvider -R -x $(HOSTID)
	$(PROGRAM) a -c Test_CLITestProviderClass.Id=\"Mike\" -n test/TestProvider -R -x $(HOSTID)
	$(PROGRAM) an -c Test_CLITestProviderClass.Id=\"Mike\" -n test/TestProvider -R -x $(HOSTID)

# test for invalid objecPath (class only) on associaton requests
invalidAssocObjectPath:
	$(PROGRAM) r -c Test_Person -R $(HOSTID)
	$(PROGRAM) rn -c Test_Person -R $(HOSTID)
	$(PROGRAM) a -c Test_Person -R $(HOSTID)
	$(PROGRAM) an -c Test_Person -R $(HOSTID)

################################
#### Close tests
### Set the the size and value of responses to get large number of
### responses. Then call pullop to get small set of objects and then
### close (the -X option which issues a close after 4 pull operations)
testenumclose:
	cimcli im $(CStressCxx) set -n $(NStressCxx) Size=10000 ResponseCount=10000 $(CIMCLIHOSTID)
	$(PROGRAM) e -c $(CStressCxx) -n $(NStressCxx) -M 9 -N 10 -X 4 $(HOSTID)

testenumpathsclose:
	cimcli im $(CStressCxx) set -n $(NStressCxx) Size=10000 ResponseCount=10000 $(CIMCLIHOSTID)
	$(PROGRAM) en -c $(CStressCxx) -n $(NStressCxx) -M 9 -N 10 -X 4 $(HOSTID)

#################################
#### timeout test. Set interoptime to 2 sec and sleep to 10 sec. This
#### test should timeout after the first pull. (-s defines time to sleep
#### between operations)
testtimeout:
	cimcli im $(CStressCxx) set -n $(NStressCxx) Size=100 ResponseCount=1000 $(CIMCLIHOSTID)
	$(PROGRAM) e -c $(CStressCxx) -n $(NStressCxx) -M 1 -N 1 -t 2 -T -t 5 -s 10 -R $(HOSTID)

#################################
#### zeroRequest test. Set count on maxObjectCount for pull operations to zero
#### Should get exception after about 1000 pull requests.
testexceedMaxObjectCountLimit:
	cimcli im $(CStressCxx) set -n $(NStressCxx) Size=10000 ResponseCount=10000 $(CIMCLIHOSTID)
	$(PROGRAM) e -c $(CStressCxx) -n $(NStressCxx) -M 1 -N 0 -T -R $(HOSTID)

##################################
## CMPI Provider tests
pcmpia1:
	$(PROGRAM) a -M 1 -N 1 -c $(CMPIASSOCOBJ) -n test/TestProvider -C -T $(HOSTID)

pcmpian1:
	$(PROGRAM) an -M 1 -N 1 -c $(CMPIASSOCOBJ) -n test/TestProvider -C -T $(HOSTID)

####################################################
##
##     Manual Test targets used during development
##nstanceWithPath
###################################################

## test all operations against all classes in namespace root/cimv2
allOperationsCIMV2:
	$(PROGRAM) all -n root/cimv2 -C -T -v 3 $(HOSTID)

allOperationsInterop:
	$(PROGRAM) all -n root/PG_InterOp -C -T -v 3 $(HOSTID)

clirep:
	cimcli pei Test_Person -n test/TestProvider $(CIMCLIHOSTID)

cliprov:
	cimcli pei Sample_InstanceProviderClass -n root/SampleProvider $(CIMCLIHOSTID)

pr: pr1 pr2

e1:
	cimcli ei Sample_InstanceProviderClass -n root/SampleProvider $(CIMCLIHOSTID)

e2:
	cimcli ei Test_Person -n test/TestProvider $(CIMCLIHOSTID)

r2:
	cimcli r Test_CLITestProviderClass.Id=\"Mike\" -n test/TestProvider $(CIMCLIHOSTID)

pe: pullOneEnumerateInstances pe2

## Tests for reference names and pull referencenames.
r1:
	cimcli r Test_CLITestProviderClass.Id=\"Mike\" -n test/TestProvider $(CIMCLIHOSTID)
rn1:
	cimcli rn Test_CLITestProviderClass.Id=\"Mike\" -n test/TestProvider $(CIMCLIHOSTID)
an1:
	cimcli an Test_CLITestProviderClass.Id=\"Mike\" -n test/TestProvider $(CIMCLIHOSTID)

##########################
##
## manual check only since we do not have way to negate return value for
## cimcli calls.
checkinvalidroleparam:
	cimcli a CIM_Door -r help
	- cimcli a CIM_Door -r ====
	cimcli a CIM_Door -rr help
	- cimcli a CIM_Door -rr ====
	cimcli an CIM_Door -r help
	- cimcli an CIM_Door -r ====
	cimcli an CIM_Door -rr help
	- cimcli an CIM_Door -rr ====
	cimcli r CIM_Door -r help
	- cimcli r CIM_Door -r ====
	cimcli rn CIM_Door -r help
	- cimcli rn CIM_Door -r ====


################################
## Tests with StressTestProvider

rsgc:
	cimcli gc $(CStressCxx) -n $(NStressCxx) $(CIMCLIHOSTID)

rsget:
	cimcli im $(CStressCxx) get -n $(NStressCxx) $(CIMCLIHOSTID)

rsset:
	cimcli im $(CStressCxx) set -n $(NStressCxx) Size=10000 ResponseCount=10000 $(CIMCLIHOSTID)

rsset1:
	cimcli im set $(CStressCxx) -n $(NStressCxx) $(CIMCLIHOSTID)

## cimcli enumerateInstances
rsei: rsset
	cimcli ei $(CStressCxx) get -n $(NStressCxx) --sum --t $(CIMCLIHOSTID)

rseip:
	cimcli pei $(CStressCxx) get -n $(NStressCxx) --sum --t $(CIMCLIHOSTID)

## pull the number of objects defined by rsset with Timing and comparison
rseip1: rsset
	$(PROGRAM) e -c $(CStressCxx) -n $(NStressCxx) -M 9 -N 100 -T -C $(HOSTID)

rseipn:
	cimcli ni $(CStressCxx) get -n $(NStressCxx) --sum $(CIMCLIHOSTID)

##################################################
## Test with pull operations in cimcli
cimclipei:
	cimcli pei $(CStressCxx) -n $(NStressCxx) -mo 1 --sum $(CIMCLIHOSTID)
	cimcli pni $(CStressCxx) -n $(NStressCxx) -mo 1 --sum  $(CIMCLIHOSTID)

cmpigc:
	cimcli gc $(CMPICL) -n root/SampleProvider $(CIMCLIHOSTID)

## NONTest (i.e. no test criteria) of CMPI provider enumerate Instances
## DELETE THIS
cmpie1:
	cimcli pei $(CMPICL) -n root/SampleProvider $(CIMCLIHOSTID)
	cimcli pei $(CMPICL) -n root/SampleProvider $(CIMCLIHOSTID) -pl Message
	cimcli pei $(CMPICL) -n root/SampleProvider $(CIMCLIHOSTID) -pl ""


## reference tests
clicmpir1:
	cimcli pr $(CMPIASSOCOBJ) -n test/TestProvider $(CIMCLIHOSTID)

pcmpir1:
	$(PROGRAM) r -M 1 -N 1 -c $(CMPIASSOCOBJ) -n test/TestProvider -C -T $(HOSTID)

pcmpirn1:
	$(PROGRAM) rn -M 1 -N 1 -c $(CMPIASSOCOBJ) -n test/TestProvider -C -T  $(HOSTID)

## Reference Operations Simple tests

clicmpirn1:
	cimcli rn $(CMPIASSOCOBJ) -n test/TestProvider $(CIMCLIHOSTID)

clicmpipr1:
	cimcli r $(CMPIASSOCOBJ) -n test/TestProvider $(CIMCLIHOSTID)

clicmpiprn1:
	cimcli rn $(CMPIASSOCOBJ) -n test/TestProvider $(CIMCLIHOSTID)

### Known error conditions

clicmpir2:
	cimcli r CMPI_TEST_Person.name=\"Melvin\" -rc CMPI_TEST_Racing -n test/TestProvider $(CIMCLIHOSTID)
