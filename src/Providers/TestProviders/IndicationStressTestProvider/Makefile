ROOT = $(PEGASUS_ROOT)

DIR = Providers/TestProviders/IndicationStressTestProvider

include $(ROOT)/mak/config.mak

LIBRARY = IndicationStressTestProvider

LIBRARIES = \
	pegcommon \
	pegprovider \
	pegqueryexpression
	
SOURCES = \
	IndicationStressTestProviderMain.cpp \
	IndicationStressTestProvider.cpp

include $(ROOT)/mak/library.mak

tests:
