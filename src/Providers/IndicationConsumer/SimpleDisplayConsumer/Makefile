ROOT = ../../../..

DIR = Providers/IndicationConsumer/SimpleDisplayConsumer

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_CONSUMER_INTERNAL -DPEGASUS_INTERNALONLY -DINDICATION_DIR=\"$(DISPLAYCONSUMER_DIR)\"

LIBRARY = SimpleDisplayConsumer

LIBRARIES = \
	 pegcommon
	
SOURCES = \
   SimpleDisplayConsumerMain.cpp \
   SimpleDisplayConsumer.cpp

include $(ROOT)/mak/library.mak
