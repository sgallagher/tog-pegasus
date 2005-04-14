 ROOT = $(PEGASUS_ROOT)

include $(ROOT)/mak/config.mak

DIRS = \
    CLITestClients \
	CGIClient \
    TestClient \
    cliutils \
    cimconfig \
    cimuser \
    cimauth \
    cimprovider \
    cimprovider/tests \
    osinfo \
    wbemexec \
    tomof \
    ipinfo \
    g11ntest \
    benchmarkTest \
    cimperf \
    MCCATestClient

ifndef PEGASUS_XML_ORDERING_DEFECT_ENCOUNTERED
  DIRS += \
    wbemexec/tests
endif

ifeq ($(OS),HPUX)
  DIRS += \
    ntpinfo \
    dnsinfo \
    nisinfo
endif


#The following tests are NOT included as part of the standard
#regession test.  The cimuser tests require PrivilegedUser
#access to execute successfully.  The cimconfig tests modify
#configuration files.

XDIRS = \
    cimconfig/tests \
    cimuser/tests 

include $(ROOT)/mak/recurse.mak
