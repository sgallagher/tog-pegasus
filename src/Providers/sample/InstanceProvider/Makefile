ROOT = ../../../..
include ../../libraries.mak

DIR = Providers/sample/InstanceProvider

include $(ROOT)/mak/config.mak

LIBRARY = SampleInstanceProvider

LIBRARIES = \
	 $(LIB_DIR)/$(LIB_PREFIX)pegcommon$(LIB_SUFFIX) \
	 $(LIB_DIR)/$(LIB_PREFIX)pegprovider$(LIB_SUFFIX) \
	 $(LIB_DIR)/$(LIB_PREFIX)pegprovider2$(LIB_SUFFIX)
	
DYNAMIC_LIBRARIES = \
	 -lpegcommon \
	 -lpegprovider \
	 -lpegprovider2
	
SOURCES = \
   InstanceProviderMain.cpp \
   InstanceProvider.cpp

include $(ROOT)/mak/library.mak

tests:
