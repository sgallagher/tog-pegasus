ROOT = $(PEGASUS_ROOT)

DIR = Providers/sample/RT_Indication/RT_IndicationConsumer

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_CONSUMER_INTERNAL -DPEGASUS_INTERNALONLY

LIBRARY = RT_IndicationConsumer

LIBRARIES = \
	 pegcommon \
	 pegprovider
	
SOURCES = \
   RT_IndicationConsumerMain.cpp \
   RT_IndicationConsumer.cpp 

include $(ROOT)/mak/library.mak

tests:
