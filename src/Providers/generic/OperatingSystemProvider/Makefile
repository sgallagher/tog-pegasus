ROOT = ../../../..

DIR = Providers/generic/OperatingSystemProvider

include $(ROOT)/mak/config.mak

LIBRARY = OperatingSystemProvider

SOURCES = \
	OperatingSystemProvider.cpp \
	OperatingSystem.cpp

LIBRARIES = \
	$(LIB_DIR)/$(LIB_PREFIX)pegcommon$(LIB_SUFFIX) \
	$(LIB_DIR)/$(LIB_PREFIX)pegprovider$(LIB_SUFFIX) \
	$(LIB_DIR)/$(LIB_PREFIX)pegprovider2$(LIB_SUFFIX)

include $(ROOT)/mak/library.mak

tests:
