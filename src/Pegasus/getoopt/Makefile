ROOT = ../../..

DIR = Pegasus/getoopt

include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

ifeq ($(OS_TYPE),windows)
EXTRA_INCLUDES += -I./nt_includes
endif

LOCAL_DEFINES = -DPEGASUS_GETOOPT_INTERNAL

LIBRARY = peggetoopt

LIBRARIES = pegcommon

PRE_DEPEND_INCLUDES = -I./depends

SOURCES = getoopt.cpp

OBJECTS = $(SOURCES:.cpp=$(OBJ))

include $(ROOT)/mak/library.mak

