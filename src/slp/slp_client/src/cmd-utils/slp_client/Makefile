ROOT=$(PEGASUS_ROOT)

DIR = slp/slp_client

include $(ROOT)/mak/config.mak

# Treat all the SOURCES as C source only
ifeq ($(PEGASUS_PLATFORM),WIN32_IX86_MSVC)
FLAGS += /TC
endif
ifeq ($(PEGASUS_PLATFORM),LINUX_IX86_GNU)
FLAGS += -x c 
endif

LIBRARY = slp_client

SOURCES = \
    slp_client.cpp \
    lslp.cpp \
    y_attr.cpp \
    l_attr.cpp \
    y_url.cpp \
    l_url.cpp \
    y_filter.cpp \
    l_filter.cpp

ifeq ($(PEGASUS_PLATFORM),WIN32_IX86_MSVC)
  SYS_LIBS = ws2_32.lib advapi32.lib
endif

include $(ROOT)/mak/library.mak

# DO NOT DELETE
