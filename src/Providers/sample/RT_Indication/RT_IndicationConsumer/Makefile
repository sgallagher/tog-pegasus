ROOT = $(PEGASUS_ROOT)

DIR = Providers/sample/RT_Indication/RT_IndicationConsumer

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_CONSUMER_INTERNAL -DPEGASUS_INTERNALONLY

LIBRARY = RT_IndicationConsumer

LIBRARIES = \
    pegprovider \
    pegclient \
    pegcommon
	
SOURCES = \
   RT_IndicationConsumerMain.cpp \
   RT_IndicationConsumer.cpp 

include $(ROOT)/mak/dynamic-library.mak

tests:
