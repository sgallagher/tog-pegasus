ROOT = ${PEGASUS_ROOT}
 

DIR = $ROOT/src/Providers/slp

DEFINES += -DPEGASUS_DEBUG
include $(ROOT)/mak/config.mak

LIBRARY = SLPProvider

LIBRARIES = \
  pegcommon \
         pegconfig \
  pegprovider

SOURCES = \
   SLPProvider.cpp

include $(ROOT)/mak/library.mak

ifeq ($(PEGASUS_PLATFORM),LINUX_IX86_GNU)
    EXTRA_LIBRARIES += -luuid
    EXTRA_LIBRARIES += -L$(PEGASUS_ROOT)/lib -lslp
endif


tests:

