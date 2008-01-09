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

PEGASUS_ZOS_PROGRAM_OBJECT = yes

DIR = Clients/g11ntest
include $(ROOT)/mak/config.mak

LIBRARIES = \
    peglistener \
    pegexportserver \
    pegconfig \
    pegclient \
    pegcommon

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = g11ntest
PROGRAM_ICU = g11ntest -reqlang en -rsplang en
PROGRAM_NOICU = g11ntest
SOURCES = g11ntest.cpp

ifneq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
  ifneq ($(OS),HPUX)
    ifeq ($(PEGASUS_ENABLE_SLP),true)
       LIBRARIES += pegslp
    endif
  endif
endif

include $(ROOT)/mak/program.mak

tests:

#
# Note:  The cimserver must have forceProviderProcesses=false configured for
# this test to be successful, because the LocalizedProvider that is exercised
# by this test modifies cimserver global settings.
#
poststarttests:
	@$(ECHO) "Skipping g11ntest.  Use 'make g11ntest' to run this test."

g11ntest:
ifdef PEGASUS_HAS_MESSAGES
	$(PROGRAM_ICU)
else
	$(PROGRAM_NOICU)
endif

# IPv6 Tests.

ifeq ($(PEGASUS_TEST_IPV6), true)
	@$(ECHO) "Testing with IPv6 loop-back address."        
	$(PROGRAM) [::1]:5988 -l [::1]:2003
endif

#
# Testing for ipv4 mapped ipv6 addresses. 
# ATTN: These tests are not avilable on windows platforms as windows does not
# support ipv4 mapped ipv6 addresses in XP and 2003. At present these tests
# are enabled only on UNIX platforms.
#

ifeq ($(OS_TYPE), unix)
ifeq ($(PEGASUS_TEST_IPV6), true)
	@$(ECHO) "Testing with IPv4 Mapped IPv6 Addresses."
	$(PROGRAM) [::ffff:127.0.0.1]:5988 -l ::ffff:127.0.0.1
endif
endif
