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
NS = test/testProvider
CL = TST_ResponseStressTest

## Simple C++ Provider classes and namespaces
CxxNS = root/SampleProvider
CxxCL = Sample_InstanceProviderClass
## C++ association Provider Object and Namespace
CxxAssocOB = Test_CLITestProviderClass.Id=\"Mike\"
CxxAssocNS = test/TestProvider

## Unit test. This is not a compelete test since this is a test program
## Simply assures that we do not crash when help executed.
tests:
	pullop -h
	pullop --help
## Tests conducted with pullop
## We are moving the test so that many of them will be direct rather than
## through makefile calls.  Will give us better control.  This is start
##of that but commented out for now
#poststartests1:
#    @$(ECHO) Error Tests
#    make filterParamError
#    make pullClassParamError
#    make continueOnErrorError
#    make invalidAssocObjectPath
#    @$(ECHO) Enumerate and EnumerateNames tests

## KS_TODO - Redo this list to actually put in the pullop code for each
## Will make it more understandable and also can work with options easier.
poststarttests:
	make filterParamError
	make pullClassParamError
	make continueOnErrorError
	make invalidAssocObjectPath

	@$(ECHO)  Enumerate and EnumerateNames tests - get to Completion
	make pe1
	make pn1
	make peall
	make penall
	make pcmpie1
	make pcmpien1
	make enumerateMaxObjectSizeVariations
	make enumeratePathsMaxObjectSizevariations
	make stresse1
	make stressen1
	@$(ECHO)  Associator and Reference tests - get to completion      
	make pr1
	make prn1
	make pa1
	make pan1
	make pcmpir1
	make pcmpirn1
	make pcmpian1
	make pcmpia1

	@$(ECHO)  Test close in process of delivery  
	make testclose
	@$(ECHO)  Test timeout in process of delivery 
	make testtimeout

	@$(ECHO) +++++ All pullop Pull Operation tests Passed.

localtests:

stresstests:

## enumerate a class that is probably all repository
pe1:
	$(PROGRAM) e -M 1 -N 1 -c $(CxxCL) -n $(CxxNS) -C -T

## test variations of open max object count and pull max object count
enumerateMaxObjectSizeVariations:
	$(PROGRAM) e -M 1 -N 1 -c $(CxxCL) -n $(CxxNS) -C -T
	$(PROGRAM) e -M 2 -N 2 -c $(CxxCL) -n $(CxxNS) -C -T
	$(PROGRAM) e -M 3 -N 3 -c $(CxxCL) -n $(CxxNS) -C -T
	$(PROGRAM) e -M 0 -N 1 -c $(CxxCL) -n $(CxxNS) -C -T
	$(PROGRAM) e -M 0 -N 3 -c $(CxxCL) -n $(CxxNS) -C -T
	$(PROGRAM) e -M 0 -N 100 -c $(CxxCL) -n $(CxxNS) -C -T
	$(PROGRAM) e -M 100 -N 10 -c $(CxxCL) -n $(CxxNS) -C -T
## test variations of open max object count and pull max object count
enumeratePathsMaxObjectSizevariations:
	$(PROGRAM) en -M 1 -N 1 -c $(CxxCL) -n $(CxxNS) -C -T
	$(PROGRAM) en -M 2 -N 2 -c $(CxxCL) -n $(CxxNS) -C -T
	$(PROGRAM) en -M 3 -N 3 -c $(CxxCL) -n $(CxxNS) -C -T
	$(PROGRAM) en -M 0 -N 1 -c $(CxxCL) -n $(CxxNS) -C -T
	$(PROGRAM) en -M 0 -N 3 -c $(CxxCL) -n $(CxxNS) -C -T
	$(PROGRAM) en -M 0 -N 100 -c $(CxxCL) -n $(CxxNS) -C -T
	$(PROGRAM) en -M 100 -N 10 -c $(CxxCL) -n $(CxxNS) -C -T

## enumerate a class that is probably all repository
pn1:
	$(PROGRAM) en -M 1 -N 1 -c $(CxxCL) -n $(CxxNS) -C

## Simple pull ref and assoc operations.
pr1:
	pullop r  -N 1 -N 1 -c $(CxxAssocOB) -n $(CxxAssocNS) -C

prn1:
	pullop rn -c Test_CLITestProviderClass.Id=\"Mike\" -n test/TestProvider -C

pa1:
	pullop a -c Test_CLITestProviderClass.Id=\"Mike\" -n test/TestProvider -C

pan1:
	pullop an -c Test_CLITestProviderClass.Id=\"Mike\" -n test/TestProvider -C


peall:
	$(PROGRAM) e -M 9 -N 9 -c CIM_ManagedElement -C

## do all objects in the test/TestProvider namespace
peall2:
	$(PROGRAM) e -M 9 -N 9 -C -c CIM_ManagedElement -n test/TestProvider

removefornow:
	$(PROGRAM) e -M 9 -N 1 -C -c CIM_ManagedElement -n root/PG_InterOp

penall:
	$(PROGRAM) e -M 9 -N 1 -C -c CIM_ManagedElement
	$(PROGRAM) e -M 9 -N 1 -C -c CIM_ManagedElement -n root/PG_InterOp

## Test Empty response.
pempty:
	pullop e -c CIM_ConcreteIdentity -n root/cimv2

## test with large scale response provider
## Execute the large scale test with just one property in response.
## issue because there is one property that may differ between instances
## the interval property that defines intervals between the delivery
## of subsequent instances to the CIMOM and therefore is not constant.
## Included list of properties to return to make this test valid.
stresse1:
	cimcli im $(CL) set -n $(NS) Size=10000 ResponseCount=10000
	pullop e -c $(CL) get -n $(NS) -C -L Pattern -L s1 -L Id -L SequenceNumber -T -v 3

stressen1:
	cimcli im $(CL) set -n $(NS) Size=10000 ResponseCount=10000
	pullop en -c $(CL) -n $(NS) -C -L Pattern -L s1 -L Id -L SequenceNumber -T -v 3

stressVariation:
	cimcli im $(CL) set -n $(NS) Size=10000 ResponseCount=10000
	pullop e -c $(CL) -n $(NS) -M 10 -N 10 -C -L Pattern -L s1 -L Id -L SequenceNumber -T -v 3
	pullop e -c $(CL) -n $(NS) -M 100 -N 100 -C -L Pattern -L s1 -L Id -L SequenceNumber -T -v 3
	pullop e -c $(CL)  -n $(NS) -M 1000 -N 1000 -C -L Pattern -L s1 -L Id -L SequenceNumber -T -v 3

	pullop e -c $(CL)  -n $(NS) -M 10000 -N 10000 -C -L Pattern -L s1 -L Id -L SequenceNumber -T -v 3
### Error Tests 
## These tests actually return exit 0 because the -R option reverses the
## exit codes so the following should actually return positive exits for
## make
pullClassParamError:
	pullop e -c blah -R
	pullop en -c blah -R

# the following tests should pass Makefile exit error tests.
filterParamError:
	pullop e Test_Person -f abc -R
	pullop e Test_Person -l WQL -R
	pullop en Test_Person -f abc -R
	pullop en Test_Person -l WQL -R
	pullop r Test_Person -f abc -R
	pullop r Test_Person -l WQL -R
	pullop rn Test_Person -f abc -R
	pullop rn Test_Person -l WQL -R
	pullop a Test_Person -f abc -R
	pullop a Test_Person -l WQL -R
	pullop an Test_Person -f abc -R
	pullop an Test_Person -l WQL -R

# test continueOnError Parameter.  Should return error in all cases
continueOnErrorError:
	pullop e Test_Person  -R -x
	pullop en Test_Person  -R -x
	pullop r -c Test_CLITestProviderClass.Id=\"Mike\" -n test/TestProvider -R -x
	pullop rn -c Test_CLITestProviderClass.Id=\"Mike\" -n test/TestProvider -R -x
	pullop a -c Test_CLITestProviderClass.Id=\"Mike\" -n test/TestProvider -R -x
	pullop an -c Test_CLITestProviderClass.Id=\"Mike\" -n test/TestProvider -R -x

# test for invalid objecPath (class only) on associaton requests
invalidAssocObjectPath:
	pullop r -c Test_Person -R
	pullop rn -c Test_Person -R
	pullop a -c Test_Person -R
	pullop an -c Test_Person -R

################################
#### Close tests
testclose: 
	cimcli im $(CL) set -n $(NS) Size=10000 ResponseCount=1000
	pullop e -c $(CL) -n $(NS) -M 9 -N 50 -o 4

#################################
#### timeout test. Set interoptime to 5 sec and sleep to 10 sec. This
#### test should timeout after with the first pull.
testtimeout:
	cimcli im $(CL) set -n $(NS) Size=10000 ResponseCount=1000
	pullop e -c $(CL) -n $(NS) -M 1 -N 1 -t 2 -T -s 4 -R

##################################
## CMPI Provider tests
pcmpia1:
	$(PROGRAM) a -M 1 -N 1 -c $(CMPIASSOCOBJ) -n test/TestProvider -C -T

pcmpian1:
	$(PROGRAM) an -M 1 -N 1 -c $(CMPIASSOCOBJ) -n test/TestProvider -C -T

####################################################
##
##     Manual Test targets used during development
##
###################################################

all:
	pullop all -n root/cimv2 -C -T -v 3

clirep:
	cimcli pei Test_Person -n test/TestProvider

cliprov:
	cimcli pei Sample_InstanceProviderClass -n root/SampleProvider

e1:
	cimcli ei Sample_InstanceProviderClass -n root/SampleProvider

e2:
	cimcli ei Test_Person -n test/TestProvider

r2:
	cimcli r Test_CLITestProviderClass.Id=\"Mike\" -n test/TestProvider

pr: pr1 pr2

pe: pe1 pe2

pr2:
	pullop r -n test/TestProvider -c Test_CLITestProviderClass.Id=\"Mike\" -C

pr3:
	pullop r -M 9 -N 1 -n test/TestProvider -c Test_CLITestProviderClass.Id=\"Mike\"

## Tests for reference names and pull referencenames.
r1:
	cimcli r Test_CLITestProviderClass.Id=\"Mike\" -n test/TestProvider
rn1:
	cimcli rn Test_CLITestProviderClass.Id=\"Mike\" -n test/TestProvider
an1:
	cimcli an Test_CLITestProviderClass.Id=\"Mike\" -n test/TestProvider

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
	cimcli gc $(CL) -n $(NS)

rsget:
	cimcli im $(CL) get -n $(NS)

rsset:
	cimcli im $(CL) set -n $(NS) Size=10000 ResponseCount=10000

rsset1:
	cimcli im set $(CL) -n $(NS)

## cimcli enumerateInstances
rsei: rsset
	cimcli ei $(CL) get -n $(NS) --sum --t

rseip:
	cimcli pei $(CL) get -n $(NS) --sum --t

## pull the number of objects defined by rsset with Timing and comparison
rseip1: rsset
	pullop e -c $(CL) -n $(NS) -M 9 -N 100 -T -C

rseipn:
	cimcli ni $(CL) get -n $(NS) --sum

##################################################
## Test with pull operations in cimcli
cimclipei:
	cimcli pei $(CL) -n $(NS) -mo 1 --sum
	cimcli pni $(CL) -n $(NS) -mo 1

cmpigc:
	cimcli gc $(CMPICL) -n root/SampleProvider

cmpie1:
	cimcli ei $(CMPICL) -n root/SampleProvider

pcmpie1:
	$(PROGRAM) e -M 1 -N 1 -c $(CMPICL) -n root/SampleProvider -C -T

pcmpien1:
	$(PROGRAM) e -M 1 -N 1 -c $(CMPICL) -n root/SampleProvider -C -T

## reference tests
cmpir1:
	cimcli r $(CMPIASSOCOBJ) -n test/TestProvider

pcmpir1:
	$(PROGRAM) r -M 1 -N 1 -c $(CMPIASSOCOBJ) -n test/TestProvider -C -T

pcmpirn1:
	$(PROGRAM) rn -M 1 -N 1 -c $(CMPIASSOCOBJ) -n test/TestProvider -C -T

## Association tests
cmpia1:
	cimcli a $(CMPIASSOCOBJ) -n test/TestProvider


