ROOT = ../../../..

DIR = Providers/IndicationConsumer/SimpleDisplayConsumer

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_CONSUMER_INTERNAL -DPEGASUS_INTERNALONLY -DINDICATION_DIR=\"$(DISPLAYCONSUMER_DIR)\"

LIBRARY = SimpleDisplayConsumer

LIBRARIES = \
    pegprovider \
    pegclient \
    pegcommon
	
SOURCES = \
   SimpleDisplayConsumerMain.cpp \
   SimpleDisplayConsumer.cpp

ifeq ($(OS),VMS)
#
# VMS uses different escape characters for including double quotes.
#
LOCAL_DEFINES = -DPEGASUS_CONSUMER_INTERNAL -DINDICATION_DIR="""$(DISPLAYCONSUMER_DIR)""" -DPEGASUS_INTERNALONLY
endif

include $(ROOT)/mak/dynamic-library.mak

install:
