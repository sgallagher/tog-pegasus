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
    tomof

ifeq ($(OS),HPUX)
  DIRS += \
    ntpinfo \
    dnsinfo
endif


#The following tests are NOT included as part of the standard
#regession test.  The cimuser tests require PrivilegedUser
#access to execute successfully.  The cimconfig tests modify
#configuration files.

XDIRS = \
    cimconfig/tests \
    cimuser/tests 

include $(ROOT)/mak/recurse.mak
