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

HOSTID = -H localhost
CIMCLIHOSTID = -l localhost

## Unit test. This is not a compelete test since this is a test program
## Simply assures that we do not crash when help executed.
tests:
	pullop -h
	$(PROGRAM) --help

## Tests conducted with pullop
## We are moving the test so that many of them will be direct rather than
## through makefile calls.  Will give e -M 1 -N 1 -c $(CxxCL) -n $(CxxNS) -C -T $(HOSTID)us better control.  This is start
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
	make testEnumerateInstances
	make testEnumerateInstanceNames
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
	matestPullEnumerateInstanceNameske pan1
	make pcmpir1
	make pcmpirn1
	make pcmpian1
	make pcmpia1

	@$(ECHO)  Test close in process of delivery  
	make testenumclose
	make testenumpathsclose

	@$(ECHO)  Test timeout in process of delivery 
	make testtimeout

	@$(ECHO) +++++ All pullop Pull Operation tests Passed.

## enumerate a class that is probably all repository
testEnumerateInstances:
	$(PROGRAM) e -M 5 -N 5 -c $(CxxCL) -n $(CxxNS) -C -T $(HOSTID)

## KS_TODO - This does now work right now.
testEnumerateInstancesWithPropertyList:
	$(PROGRAM) e -M 5 -N 5 -c $(CxxCL) -n $(CxxNS) -C -T $(HOSTID) -P Message

## KS_TODO - This does now work right now.
testEnumerateInstancesEmptyPropertyList:
	$(testPullEnumerateInstanceNamesPROGRAM) e -M 5 -N 5 -c $(CxxCL) -n $(CxxNS) -C -T $(HOSTID) -P ""

## test variations of open max object count and pull max object count
enumerateMaxObjectSizeVariations:
	$(PROGRAM) e -M 1 -N 1 -c $(CxxCL) -n $(CxxNS) -C -T $(HOSTID)
	$(PROGRAM) e -M 2 -N 2 -c $(CxxCL) -n $(CxxNS) -C -T $(HOSTID)
	$(PROGRAM) e -M 3 -N 3 -c $(CxxCL) -n $(CxxNS) -C -T $(HOSTID)
	$(PROGRAM) e -M 0 -N 1 -c $(CxxCL) -n $(CxxNS) -C -T $(HOSTID)
	$(PROGRAM) e -M 0 -N 3 -c $(CxxCL) -n $(CxxNS) -C -T $(HOSTID)
	$(PROGRAM) e -M 0 -N 100 -c $(CxxCL) -n $(CxxNS) -C -T $(HOSTID)
	$(PROGRAM) e -M 100 -N 10 -c $(CxxCL) -n $(CxxNS) -C -T $(HOSTID)
## test variations of open max object count and pull max object count
enuWith Pull ValidationmeratePathsMaxObjectSizevariations:
	$(PROGRAM) en -M 1 -N 1 -c $(CxxCL) -n $(CxxNS) -C -T $(HOSTID)
	$(PROGRAM) en -M 2 -N 2 -c $(CxxCL) -n $(CxxNS) -C -T $(HOSTID)
	$(PROGRAM) en -M 3 -N 3 -c $(CxxCL) -n $(CxxNS) -C -T $(HOSTID)
	$(PROGRAM) en -M 0 -N 1 -c $(CxxCL) -n $(CxxNS) -C -T $(HOSTID)
	$(PROGRAM) en -M 0 -N 3 -c $(CxxCL) -n $(CxxNS) -C -T $(HOSTID)
	$(PROGRAM) en -M 0 -N 100 -c $(CxxCL) -n $(CxxNS) -C -T $(HOSTID)
	$(PROGRAM) en -M 100 -N 10 -c $(CxxCL) -n $(CxxNS) -C -T $(HOSTID)

## enumerate instance names of a class that is probably all repository
testEnumerateInstanceNames:
	$(PROGRAM) en -M 1 -N 1 -c $(CxxCL) -n $(CxxNS) -C $(HOSTID)

## Simple pull ref and assoc operations.
pr1:
	$(PROGRAM) r  -N 1 -N 1 -c $(CxxAssocOB) -n $(CxxAssocNS) -C $(HOSTID)

prn1:
	$(PROGRAM) r  -N 1 -N 1 -c $(CxxAssocOB) -n $(CxxAssocNS) -C $(HOSTID)

pr2:
	$(PROGRAM) rn -c Test_CLITestProviderClass.Id=\"Mike\" -n test/TestProvider -C $(HOSTID)

prn2:
	$(PROGRAM) rn -c Test_CLITestProviderClass.Id=\"Mike\" -n test/TestProvider -C $(HOSTID)

pa1:
	$(PROGRAM) a -N 1 -N 1 -c $(CxxAssocOB) -n $(CxxAssocNS) -C $(HOSTID)

pan1:
	$(PROGRAM) an -N 1 -N 1 -c $(CxxAssocOB) -n $(CxxAssocNS) -C $(HOSTID)

pa2:
	$(PROGRAM) a -c Test_CLITestProviderClass.Id=\"Mike\" -n test/TestProvider -C $(HOSTID)

pan2:
	$(PROGRAM) an -c Test_CLITestProviderClass.Id=\"Mike\" -n test/TestProvider -C $(HOSTID)

## With repository entry
pa3:
	$(PROGRAM) a -N 1 -N 1 -c Person.name=\"Mike\" -n root/SampleProvider -C $(HOSTID)
pan3:
	$(PROGRAM) an -N 1 -N 1 -c Person.name=\"Mike\" -n root/SampleProvider -C $(HOSTID)

peall:
	$(PROGRAM) e -M 9 -N 9 -c CIM_ManagedElement -C $(HOSTID)

## KS_TODO this one generates an error
peall1:
	$(PROGRAM) e -M 9 -N 9 -c CIM_Namespace -C -n root/PG_InterOp $(HOSTID)

peall2:
	$(PROGRAM) e -M 9 -N 9 -C -c CIM_ManagedElement -n root/PG_InterOp $(HOSTID)

## Enumerate all objects in the test/TestProvider namespace
peall3:
	$(PROGRAM) e -M 9 -N 9 -C -c CIM_ManagedElement -n test/TestProvider $(HOSTID)

removefornow:
	$(PROGRAM) e -M 9 -N 1 -C -c CIM_ManagedElement -n root/PG_InterOp $(HOSTID)

penall:
	$(PROGRAM) en -M 9 -N 1 -C -c CIM_ManagedElement $(HOSTID)
###	$(PROGRAM) en -M 9 -N 1 -C -c CIM_ManagedElement -n root/PG_InterOp $(HOSTID)

## Test Empty response.
pempty:
	$(PROGRAM) e -c CIM_ConcreteIdentity -n root/cimv2 $(HOSTID)

## test with large scale response provider
## Execute the large scale test with just one property in response.
## issue because there is one property that may differ between instances
## the interval property that defines intervals between the delivery
## of subsequent instances to the CIMOM and therefore is not constant.
## Included list of properties to return to make this test valid.
stresse1:
	cimcli im $(CL) set -n $(NS) Size=10000 ResponseCount=10000 $(CIMCLIHOSTID)
	$(PROGRAM) e -c $(CL) get -n $(NS) -C -L Pattern -L s1 -L Id -L SequenceNumber -T -v 3 $(HOSTID)

stressen1:
	cimcli im $(CL) set -n $(NS) Size=10000 ResponseCount=10000 $(CIMCLIHOSTID)
	$(PROGRAM) en -c $(CL) -n $(NS) -C -L Pattern -L s1 -L Id -L SequenceNumber -T -v 3 $(HOSTID)

stressVariation:
	cimcli im $(CL) set -n $(NS) Size=10000 ResponseCount=10000 $(CIMCLIHOSTID)
	$(PROGRAM) e -c $(CL) -n $(NS) -M 10 -N 10 -C -L Pattern -L s1 -L Id -L SequenceNumber -T -v 3 $(HOSTID)
	$(PROGRAM) e -c $(CL) -n $(NS) -M 100 -N 100 -C -L Pattern -L s1 -L Id -L SequenceNumber -T -v 3 $(HOSTID)
	$(PROGRAM) e -c $(CL)  -n $(NS) -M 1000 -N 1000 -C -L Pattern -L s1 -L Id -L SequenceNumber -T -v 3 $(HOSTID)

	$(PROGRAM) e -c $(CL)  -n $(NS) -M 10000 -N 10000 -C -L Pattern -L s1 -L Id -L SequenceNumber -T -v 3 $(HOSTID)
### Error Tests 
## These tests actually return exit 0 because the -R option reverses the
## exit codes so the following should actually return positive exits for
## make
pullClassParamError:
	$(PROGRAM) e -c blah -R $(HOSTID)
	$(PROGRAM) en -c blah -R $(HOSTID)

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

# test continueOnError Parameter.  Should return error in all cases
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
### close
testenumclose: 
	cimcli im $(CL) set -n $(NS) Size=10000 ResponseCount=1000 $(CIMCLIHOSTID)
	$(PROGRAM) e -c $(CL) -n $(NS) -M 9 -N 50 -o 4 $(HOSTID)

testenumpathsclose: 
	cimcli im $(CL) set -n $(NS) Size=10000 ResponseCount=1000 $(CIMCLIHOSTID)
	$(PROGRAM) en -c $(CL) -n $(NS) -M 9 -N 50 -o 4 $(HOSTID)
pr: pr1 pr2
#################################
#### timeout test. Set interoptime to 5 sec and sleep to 10 sec. This
#### test should timeout after with the first pull.
testtimeout:
	cimcli im $(CL) set -n $(NS) Size=10000 ResponseCount=1000 $(CIMCLIHOSTID)
	$(PROGRAM) e -c $(CL) -n $(NS) -M 1 -N 1 -t 2 -T -s 4 -R $(HOSTID)

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

## test all operations against all classes in the defined namespace
all:
	$(PROGRAM) all -n root/cimv2 -C -T -v 3 $(HOSTID)

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

pr3:
	$(PROGRAM) r -M 9 -N 1 -n test/TestProvider -c Test_CLITestProviderClass.Id=\"Mike\" $(HOSTID)

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
	cimcli gc $(CL) -n $(NS) $(CIMCLIHOSTID)

rsget:
	cimcli im $(CL) get -n $(NS) $(CIMCLIHOSTID)

rsset:
	cimcli im $(CL) set -n $(NS) Size=10000 ResponseCount=10000 $(CIMCLIHOSTID)

rsset1:
	cimcli im set $(CL) -n $(NS) $(CIMCLIHOSTID)

## cimcli enumerateInstances
rsei: rsset
	cimcli ei $(CL) get -n $(NS) --sum --t $(CIMCLIHOSTID)

rseip:
	cimcli pei $(CL) get -n $(NS) --sum --t $(CIMCLIHOSTID)

## pull the number of objects defined by rsset with Timing and comparison
rseip1: rsset
	$(PROGRAM) e -c $(CL) -n $(NS) -M 9 -N 100 -T -C $(HOSTID)

rseipn:
	cimcli ni $(CL) get -n $(NS) --sum $(CIMCLIHOSTID)

##################################################
## Test with pull operations in cimcli
cimclipei:
	cimcli pei $(CL) -n $(NS) -mo 1 --sum $(CIMCLIHOSTID)
	cimcli pni $(CL) -n $(NS) -mo 1 $(CIMCLIHOSTID)

cmpigc:
	cimcli gc $(CMPICL) -n root/SampleProvider $(CIMCLIHOSTID)

cmpie1:
	cimcli ei $(CMPICL) -n root/SampleProvider $(CIMCLIHOSTID)

pcmpie1:
	$(PROGRAM) e -M 1 -N 1 -c $(CMPICL) -n root/SampleProvider -C -T $(HOSTID)

pcmpien1:
	$(PROGRAM) e -M 1 -N 1 -c $(CMPICL) -n root/SampleProvider -C -T $(HOSTID)

## reference tests
cmpir1:
	cimcli r $(CMPIASSOCOBJ) -n test/TestProvider $(CIMCLIHOSTID)

pcmpir1:
	$(PROGRAM) r -M 1 -N 1 -c $(CMPIASSOCOBJ) -n test/TestProvider -C -T $(HOSTID) 

pcmpirn1:
	$(PROGRAM) rn -M 1 -N 1 -c $(CMPIASSOCOBJ) -n test/TestProvider -C -T  $(HOSTID)

## Association tests
cmpia1:
	cimcli a $(CMPIASSOCOBJ) -n test/TestProvider $(CIMCLIHOSTID)


