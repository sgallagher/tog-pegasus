ROOT = ../..

DIR = slp

include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = 

LIBRARY = slp

PRE_DEPEND_INCLUDES = -I./depends

ifneq ($(PEGASUS_SUPPORTS_DYNLIB),yes)
    LIBRARIES = pegcommon
endif



SOURCES = peg_slp_agent.cpp 

OBJECTS = $(SOURCES:.cpp=$(OBJ))

SYS_LIBS = ws2_32.lib advapi32.lib

include $(ROOT)/mak/library.mak

$(SOURCES): common
common :
	@ $(MAKE) -SC $(ROOT)/src/Pegasus/Common

run:
	Server $(REPOSITORY_ROOT)
