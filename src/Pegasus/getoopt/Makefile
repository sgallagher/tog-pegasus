ROOT = ../../..

DIR = Pegasus/getoopt

include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

ifeq ($(OS_TYPE),windows)
EXTRA_INCLUDES += -I./nt_includes
endif

LOCAL_DEFINES = -DPEGASUS_GETOOPT_INTERNAL

LIBRARY = peggetoopt

LIBRARIES = $(LIB_DIR)/$(LIB_PREFIX)pegcommon$(LIB_SUFFIX)

ifeq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
DYNAMIC_LIBRARIES = $(LIB_DIR)/$(LIB_PREFIX)pegcommon.x
else
 DYNAMIC_LIBRARIES = -lpegcommon
endif

PRE_DEPEND_INCLUDES = -I./depends

SOURCES = getoopt.cpp

OBJECTS = $(SOURCES:.cpp=$(OBJ))

include $(ROOT)/mak/library.mak

