ROOT = ../../../..
include ../../libraries.mak

DIR = Providers/sample/IndicationProvider

include $(ROOT)/mak/config.mak

LIBRARY = SampleIndicationProvider

LIBRARIES = \
	 $(LIB_DIR)/$(LIB_PREFIX)pegcommon$(LIB_SUFFIX) \
	 $(LIB_DIR)/$(LIB_PREFIX)pegprovider$(LIB_SUFFIX) \
	 $(LIB_DIR)/$(LIB_PREFIX)pegprovider2$(LIB_SUFFIX)
	
ifeq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
DYNAMIC_LIBRARIES = \
	 $(LIB_DIR)/$(LIB_PREFIX)pegcommon.x \
	 $(LIB_DIR)/$(LIB_PREFIX)pegprovider.x \
	 $(LIB_DIR)/$(LIB_PREFIX)pegprovider2.x
else
DYNAMIC_LIBRARIES = \
	 -lpegcommon \
	 -lpegprovider \
	 -lpegprovider2
endif
	
SOURCES = \
   IndicationProviderMain.cpp \
   IndicationProvider.cpp

include $(ROOT)/mak/library.mak

tests:
