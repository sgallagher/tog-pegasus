ROOT = ../../..

DIR = Pegasus/Listener

include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_LISTENER_INTERNAL

LIBRARY = peglistener

LIBRARIES = \
    pegcommon \
    pegexportserver \
    pegconfig 

PRE_DEPEND_INCLUDES = -I./depends

SOURCES = \
    CIMListener.cpp

OBJECTS = $(SOURCES:.cpp=$(OBJ))

SYS_LIBS = ws2_32.lib advapi32.lib

include $(ROOT)/mak/library.mak

run:
	Listener $(REPOSITORY_ROOT)
