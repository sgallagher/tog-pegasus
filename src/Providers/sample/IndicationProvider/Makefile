ROOT = ../../../..
include ../../libraries.mak

DIR = Providers/sample/IndicationProvider

include $(ROOT)/mak/config.mak

LIBRARY = SampleIndicationProvider

LIBRARIES = \
	 $(LIB_DIR)/$(LIB_PREFIX)pegcommon$(LIB_SUFFIX) \
	 $(LIB_DIR)/$(LIB_PREFIX)pegprovider$(LIB_SUFFIX) \
	 $(LIB_DIR)/$(LIB_PREFIX)pegprovider2$(LIB_SUFFIX)
	
DYNAMIC_LIBRARIES = \
	 -lpegcommon \
	 -lpegprovider \
	 -lpegprovider2
	
SOURCES = \
   IndicationProviderMain.cpp \
   IndicationProvider.cpp

include $(ROOT)/mak/library.mak

tests:
