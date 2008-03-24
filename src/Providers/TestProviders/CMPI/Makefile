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

ROOT = ../../../..

include $(ROOT)/mak/config.mak

DIRS = \
    TestUtilLib \
    TestMethod \
    TestMethod/tests \
    TestError \
    TestError/tests \
    TestProperty/tests \
    TestFail_1 \
    TestFail_1/tests \
    TestFail_2 \
    TestFail_2/tests\
    TestFail_3 \
    TestFail_3/tests \
    TestFail_4 \
    TestFail_4/tests \
    TestFail_5 \
    TestFail_5/tests \
    TestProviderThreaded \
    TestProviderThreaded/tests \
    TestAssociation \
    TestAssociation/tests \
    TestIndicationStressTestProvider \
    TestIndicationStressTestProvider/tests \
    TestCMPIEnumeration \
    TestCMPIEnumeration/tests \
    TestCMPIInstance \
    TestCMPIInstance/tests 

ifeq ($(PEGASUS_ENABLE_CQL),true)
DIRS += \
    TestIndications \
    TestIndications/tests

ifdef PEGASUS_ENABLE_EXECQUERY
DIRS += \
    TestCMPIBroker \
    TestCMPIBroker/tests \
    TestCMPIBrokerEnc \
    TestCMPIBrokerEnc/tests \
    TestExecQuery \
    TestExecQuery/tests
endif
endif

include $(ROOT)/mak/recurse.mak
