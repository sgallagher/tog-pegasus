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
    MCCATestClient \
    repupgrade  \
    repupgrade/SSPModules/SampleSSPModule \
    repupgrade/tests

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

ifeq ($(OS),VMS)
  DIRS += \
    ntpinfo \
    dnsinfo \
    nisinfo
endif

DIRS_SSLTRUSTMGR = \
    ssltrustmgr 

ifdef PEGASUS_HAS_SSL
  DIRS += \
    $(DIRS_SSLTRUSTMGR) \
    ssltrustmgr/tests \
    TestCertClient
endif

#The following tests are NOT included as part of the standard
#regession test.  The cimuser tests require PrivilegedUser
#access to execute successfully.  The cimconfig tests modify
#configuration files.

XDIRS = \
    cimconfig/tests \
    cimuser/tests 

include $(ROOT)/mak/recurse.mak
