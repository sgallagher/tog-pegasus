ROOT = ../../..

DIR = Pegasus/_cimom

include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_CIMOM_INTERNAL

LIBRARY = cimom

LIBRARIES = \
    pegcommon 

PRE_DEPEND_INCLUDES = -I./depends

SOURCES = \
	CimomMessage.cpp \
	Cimom.cpp

OBJECTS = $(SOURCES:.cpp=$(OBJ))

SYS_LIBS = ws2_32.lib advapi32.lib

include $(ROOT)/mak/library.mak

run:
	Server $(REPOSITORY_ROOT)
