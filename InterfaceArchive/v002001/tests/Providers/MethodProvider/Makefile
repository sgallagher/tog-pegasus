ROOT = $(PEGASUS_ROOT)

DIR = InterfaceArchive/$(VERSION)/tests/Providers/MethodProvider

include ../../../version.mak
include $(ROOT)/mak/config.mak
LIBRARY = SampleMethodProvider

LIBRARIES = \
	pegcommon \
	pegprovider
	
SOURCES = \
	MethodProviderMain.cpp \
	MethodProvider.cpp

include $(ROOT)/mak/library.mak

ifdef PEGASUS_TEST_BACKWARD_COMPATIBILITY
  INCLUDES = -I$(ROOT)/InterfaceArchive/$(VERSION)/include
endif

tests:
