ROOT = ../../..

DIR = slp/slp_agent

include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_SLP_INTERNAL

LIBRARY = pegslp

PRE_DEPEND_INCLUDES = -I./depends

ifneq ($PEGASUS_SUPPORTS_DYNLIB,yes)
    LIBRARIES = pegcommon
endif

SOURCES = peg_slp_agent.cpp 

OBJECTS = $(SOURCES:.cpp=$(OBJ))

include $(ROOT)/mak/library.mak

#ifdef PEGASUS_ENABLE_SLP
#$(SOURCES): common
#common :
#	@ $(MAKE) -SC $(ROOT)/src/Pegasus/Common
#ifeq ($(PEGASUS_PLATFORM),WIN32_IX86_MSVC)
#SYS_LIBS = ws2_32.lib advapi32.lib
#endif
#endif

run:
	Server $(REPOSITORY_ROOT)

include $(ROOT)/mak/install.mak
