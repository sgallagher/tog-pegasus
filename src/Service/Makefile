ROOT = ../..

DIR = Service

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY -DPEGASUS_SERVICE_INTERNAL

LIBRARY = pegservice

LIBRARIES = \
	pegcommon

SOURCES = \
	ServerProcess.cpp
	
EXTRA_INCLUDES = $(SYS_INCLUDES)
       
include $(ROOT)/mak/library.mak

ifeq ($(PEGASUS_PLATFORM),WIN32_IX86_MSVC)
  SYS_LIBS = ws2_32.lib advapi32.lib	
endif


