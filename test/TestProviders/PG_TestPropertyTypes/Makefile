ROOT = ../../..

include ../libraries.mak

DIR = TestProviders/PG_TestPropertyTypes

include $(ROOT)/mak/config.mak

LIBRARY = PG_TestPropertyTypes

LIBRARIES = \
$(LIB_DIR)/$(LIB_PREFIX)pegcommon$(LIB_SUFFIX) \
$(LIB_DIR)/$(LIB_PREFIX)pegprovider$(LIB_SUFFIX) \
$(LIB_DIR)/$(LIB_PREFIX)pegprovider2$(LIB_SUFFIX)
	
DYNAMIC_LIBRARIES = \
	 -lpegcommon \
	 -lpegprovider \
         -lpegprovider2
	
SOURCES = \
   PG_TestPropertyTypes.cpp

include $(ROOT)/mak/library.mak

tests:
