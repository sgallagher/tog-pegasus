ROOT = ../../..

DIR = Pegasus/DynListener

include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_DYNLISTENER_INTERNAL -DPEGASUS_INTERNALONLY

LIBRARY = pegdynlistener

LIBRARIES = \
    pegexportserver \
    pegcommon \
    pegprovider

PRE_DEPEND_INCLUDES = -I./depends

SOURCES = \
	DynamicConsumer.cpp \
	DynamicConsumerFacade.cpp \
	DynamicListenerIndicationDispatcher.cpp \
	ConsumerManager.cpp \
	ConsumerModule.cpp \
	ListenerService.cpp \
	DynamicListenerConfig.cpp \
	DynamicListener.cpp

OBJECTS = $(SOURCES:.cpp=$(OBJ))

SYS_LIBS = ws2_32.lib advapi32.lib

include $(ROOT)/mak/library.mak
include $(ROOT)/mak/install.mak

