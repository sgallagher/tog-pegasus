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

SOURCES = slp.cpp
## TO build and test THE NEW SLP agent: comment out the previous line, 
## uncomment the next line: 
#SOURCES = peg_slp_agent.cpp slp.cpp

OBJECTS = $(SOURCES:.cpp=$(OBJ))

SYS_LIBS = ws2_32.lib advapi32.lib

include $(ROOT)/mak/library.mak

run:
	Server $(REPOSITORY_ROOT)
