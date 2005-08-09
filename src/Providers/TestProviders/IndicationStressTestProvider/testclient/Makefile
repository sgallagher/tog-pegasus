#//%2005////////////////////////////////////////////////////////////////////////
#//
#// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
#// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
#// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation, The Open Group.
#// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
#// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
#// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
ROOT = $(PEGASUS_ROOT)

DIR = Providers/TestProviders/IndicationStressTestProvider/testclient

include $(ROOT)/mak/config.mak

PEGASUS_ZOS_PROGRAM_OBJECT = yes
LIBRARIES = \
     peglistener \
     pegclient \
     pegexportserver \
     pegcommon

EXTRA_INCLUDES = $(SYS_INCLUDES)

ifeq ($(OS),VMS)
 LOCAL_DEFINES = -DINDICATION_DIR="""$(DISPLAYCONSUMER_DIR)"""
else
 LOCAL_DEFINES = -DINDICATION_DIR=\"$(DISPLAYCONSUMER_DIR)\"
endif

PROGRAM = TestIndicationStressTest

SOURCES = IndicationStressTest.cpp

include $(ROOT)/mak/program.mak

tests:

## poststarttests: wql_10_1  cql_10_1 wql_10_10 cql_10_10 wql_10_100 cql_10_100 wql_100_10 cql_100_10 

poststarttests: wql_10_1  cql_10_1 
	@$(ECHO) "+++++ TestIndicationStressTest passed all tests"

wql_10_1:
	$(PROGRAM) setup WQL
	$(PROGRAM) run 1
	$(PROGRAM) run 10 1
	$(PROGRAM) cleanup

## use the cql_i_t and wql_i_t rule as follows:
## 
## make cql_i_t i=10 t=2 
##        will run 10 iterations in 2 threads
##
## make wql_i_t i=20 t=3 
##        will run 20 iterations in 3 threads
##
cql_i_t:
	$(PROGRAM) setup CIM:CQL
	$(PROGRAM) run $i $t
	$(PROGRAM) cleanup

wql_i_t: 
	$(PROGRAM) setup WQL
	$(PROGRAM) run $i $t
	$(PROGRAM) cleanup


wql_10_10:
	$(PROGRAM) setup WQL
	$(PROGRAM) run 10 10
	$(PROGRAM) cleanup

wql_10_100:
	$(PROGRAM) setup WQL
	$(PROGRAM) run 10 100
	$(PROGRAM) cleanup

wql_100_10:
	$(PROGRAM) setup WQL
	$(PROGRAM) run 100 10
	$(PROGRAM) cleanup

wql_100_100:
	$(PROGRAM) setup WQL
	$(PROGRAM) run 100 100
	$(PROGRAM) cleanup

wql_1000_100:
	$(PROGRAM) setup WQL
	$(PROGRAM) run 1000 100
	$(PROGRAM) cleanup

cql_10_1:
	$(PROGRAM) setup CIM:CQL
	$(PROGRAM) run 1
	$(PROGRAM) run 10 1
	$(PROGRAM) cleanup

cql_10_10:
	$(PROGRAM) setup CIM:CQL
	$(PROGRAM) run 10 10
	$(PROGRAM) cleanup

cql_10_100:
	$(PROGRAM) setup CIM:CQL
	$(PROGRAM) run 10 100
	$(PROGRAM) cleanup

cql_100_10:
	$(PROGRAM) setup CIM:CQL
	$(PROGRAM) run 100 10
	$(PROGRAM) cleanup

cql_100_100:
	$(PROGRAM) setup CIM:CQL
	$(PROGRAM) run 100 100
	$(PROGRAM) cleanup

cql_1000_100:
	$(PROGRAM) setup CIM:CQL
	$(PROGRAM) run 1000 100
	$(PROGRAM) cleanup

testsclean:
	$(PROGRAM) cleanup
