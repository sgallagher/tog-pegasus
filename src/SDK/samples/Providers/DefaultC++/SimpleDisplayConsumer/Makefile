ROOT = ../../..
include $(ROOT)/mak/config.mak

LIBRARY = SimpleDisplayConsumer

SOURCE_DIR = Providers/DefaultC++/SimpleDisplayConsumer

DISPLAYCONSUMER_DIR = ./

DEFINES += -DPEGASUS_CONSUMER_INTERNAL -DINDICATION_DIR=\"$(PEGASUS_INDICATION_CONSUMER_LOG_DIR)\"

LIBRARIES = \
  pegcommon \
  pegprovider

SOURCES = \
        SimpleDisplayConsumerMain.cpp \
        SimpleDisplayConsumer.cpp

include $(ROOT)/mak/library.mak

tests:
