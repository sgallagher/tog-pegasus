ROOT = ../../../..
include ../../libraries.mak

DIR = Providers/generic/OperatingSystemProvider

include $(ROOT)/mak/config.mak

LIBRARY = OperatingSystemProvider

LIBRARIES = \
	$(LIB_DIR)/$(LIB_PREFIX)pegcommon$(LIB_SUFFIX) \
	$(LIB_DIR)/$(LIB_PREFIX)pegprovider2$(LIB_SUFFIX)

SOURCES = \
	OperatingSystem.cpp \
	OperatingSystemProvider.cpp \
	OperatingSystemProviderMain.cpp

DYNAMIC_LIBRARIES = \
	-lpegcommon \
	-lpegprovider2

include $(ROOT)/mak/library.mak
