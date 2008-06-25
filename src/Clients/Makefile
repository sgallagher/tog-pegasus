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
 ROOT = $(PEGASUS_ROOT)

include $(ROOT)/mak/config.mak

DIRS = \
    CLITestClients \
    CGIClient \
    cliutils \
    cimconfig \
    cimuser \
    cimauth \
    cimprovider \
    osinfo \
    wbemexec \
    tomof \
    ipinfo \
    cimperf \
    repupgrade  \
    cimsub \
    repupgrade/SSPModules/SampleSSPModule \
    loadxml

TEST_DIRS += \
    CGIClient/tests \
    TestClient \
    cimprovider/tests \
    g11ntest \
    benchmarkTest \
    cimsub/tests \
    repupgrade/tests \
    wbemexec/tests 

ifeq ($(OS),HPUX)
  DIRS += \
    ntpinfo \
    dnsinfo \
    nisinfo
endif

ifeq ($(OS),VMS)
  DIRS += \
    ntpinfo \
    dnsinfo \
    nisinfo
endif

ifdef PEGASUS_HAS_SSL
  DIRS += \
    TestCertClient \
    cimtrust
  TEST_DIRS += \
    cimtrust/tests
ifeq ($(PEGASUS_ENABLE_SSL_CRL_VERIFICATION),true)
  DIRS += cimcrl 
  TEST_DIRS += cimcrl/tests
endif
endif

ifeq ($(OS_TYPE),unix)
  DIRS += cimreparchive
  TEST_DIRS += cimreparchive/tests
endif

#The following tests are NOT included as part of the standard
#regession test.  The cimuser tests require PrivilegedUser
#access to execute successfully.  The cimconfig tests modify
#configuration files.

XDIRS = \
    cimconfig/tests \
    cimuser/tests 

include $(ROOT)/mak/recurse.mak
