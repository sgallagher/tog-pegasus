ROOT = ../../..

DIR = Pegasus/suballoc

include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_SUBALLOC_INTERNAL -DPEGASUS_INTERNALONLY

LIBRARY = suballoc

LIBRARIES = pegcommon 

PRE_DEPEND_INCLUDES = -I./depends

SOURCES = \
	suballoc.cpp
OBJECTS = $(SOURCES:.cpp=$(OBJ))

SYS_LIBS = ws2_32.lib advapi32.lib

AIX_LIB_PRIORITY = -100

include $(ROOT)/mak/library.mak

run:
	Server $(REPOSITORY_ROOT)
