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
    wbemexec/tests \
    tomof \
    ipinfo \
    g11ntest \
    benchmarkTest \
    cimperf \
    MCCATestClient \
    repupgrade  \
    repupgrade/SSPModules/SampleSSPModule 

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
  DIRS += $(DIRS_SSLTRUSTMGR)
endif

#The following tests are NOT included as part of the standard
#regession test.  The cimuser tests require PrivilegedUser
#access to execute successfully.  The cimconfig tests modify
#configuration files.

XDIRS = \
    cimconfig/tests \
    cimuser/tests 

include $(ROOT)/mak/recurse.mak
