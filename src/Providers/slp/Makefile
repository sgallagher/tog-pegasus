ROOT = ${PEGASUS_ROOT}

DIR = Providers/slp

DEFINES += -DPEGASUS_DEBUG

include $(ROOT)/mak/config.mak

LIBRARY = SLPProvider

LIBRARIES = \
  pegcommon \
  pegconfig \
  pegprovider\
  pegslp

SOURCES = \
   SLPProvider.cpp

include $(ROOT)/mak/library.mak

#ifeq ($(PEGASUS_PLATFORM),LINUX_IX86_GNU)
#    EXTRA_LIBRARIES += -luuid
#    EXTRA_LIBRARIES += -L$(PEGASUS_ROOT)/lib -lpegslp
#	EXTRA_LIBRARIES += -L$(PEGASUS_ROOT)/lib -lpegslp
#endif

ifeq ($(PEGASUS_PLATFORM),WIN32_IX86_MSVC)
SYS_LIBS = ole32.lib ws2_32.lib advapi32.lib
endif



tests:

