ROOT = ../../../..
DIR = Pegasus/Compiler/cmdline
include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_CMDLINE_INTERNAL

LIBRARIES = \
	$(LIB_DIR)/$(LIB_PREFIX)pegcommon$(LIB_SUFFIX) \
    	$(LIB_DIR)/$(LIB_PREFIX)pegrepository$(LIB_SUFFIX) \
	$(LIB_DIR)/$(LIB_PREFIX)pegcompiler$(LIB_SUFFIX) \
	$(LIB_DIR)/$(LIB_PREFIX)peggetoopt$(LIB_SUFFIX)

#EXTRA_INCLUDES = $(SYS_INCLUDES) $(ACE_INCLUDES)
EXTRA_INCLUDES = $(SYS_INCLUDES)

PROGRAM = cimmof

SOURCES = main.cpp cmdline.cpp cmdlineExceptions.cpp $(EXTRA_SOURCES)

include $(ROOT)/mak/program.mak


tests:
