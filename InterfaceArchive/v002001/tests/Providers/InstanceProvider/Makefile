ROOT = $(PEGASUS_ROOT)

DIR = InterfaceArchive/$(VERSION)/tests/Providers/InstanceProvider

include ../../../version.mak
include $(ROOT)/mak/config.mak
LIBRARY = SampleInstanceProvider

LIBRARIES = \
	pegcommon \
	pegprovider
	
SOURCES = \
	InstanceProviderMain.cpp \
	InstanceProvider.cpp

include $(ROOT)/mak/library.mak

ifdef PEGASUS_TEST_BACKWARD_COMPATIBILITY
  INCLUDES = -I$(ROOT)/InterfaceArchive/$(VERSION)/include
endif

tests:

