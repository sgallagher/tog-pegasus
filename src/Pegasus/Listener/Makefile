ROOT = ../../..

DIR = Pegasus/Listener

include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_LISTENER_INTERNAL -DPEGASUS_INTERNALONLY

LIBRARY = peglistener

LIBRARIES = \
    pegexportserver \
    pegcommon 

PRE_DEPEND_INCLUDES = -I./depends

SOURCES = \
	List.cpp \
    CIMListener.cpp \
	CIMListenerIndicationDispatcher.cpp

OBJECTS = $(SOURCES:.cpp=$(OBJ))

SYS_LIBS = ws2_32.lib advapi32.lib

include $(ROOT)/mak/library.mak

