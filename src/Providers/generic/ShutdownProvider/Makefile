ROOT = ../../../..
include ../../libraries.mak

DIR = Providers/generic/ShutdownProvider

include $(ROOT)/mak/config.mak

LIBRARY = ShutdownProvider

LIBRARIES = \
	 $(LIB_DIR)/$(LIB_PREFIX)pegcommon$(LIB_SUFFIX) \
	 $(LIB_DIR)/$(LIB_PREFIX)pegprovider$(LIB_SUFFIX) \
	 $(LIB_DIR)/$(LIB_PREFIX)pegprovider2$(LIB_SUFFIX) \
	 $(LIB_DIR)/$(LIB_PREFIX)pegrepository$(LIB_SUFFIX) \
	 $(LIB_DIR)/$(LIB_PREFIX)pegserver$(LIB_SUFFIX)
	
ifeq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
DYNAMIC_LIBRARIES = \
	 $(LIB_DIR)/$(LIB_PREFIX)pegcommon.x \
	 $(LIB_DIR)/$(LIB_PREFIX)pegprovider.x \
	 $(LIB_DIR)/$(LIB_PREFIX)pegprovider2.x \
	 $(LIB_DIR)/$(LIB_PREFIX)server.x
else
DYNAMIC_LIBRARIES = \
	 -lpegcommon \
	 -lpegprovider \
	 -lpegprovider2 \
	 -lpegrepository \
	 -lpegserver
endif
	
SOURCES = \
   ShutdownProvider.cpp

include $(ROOT)/mak/library.mak

tests:
