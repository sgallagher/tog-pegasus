ROOT = ../../../..
DIR = Pegasus/Compiler/cmdline
include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_CMDLINE_INTERNAL

LIBRARIES = \
	$(LIB_DIR)/$(LIB_PREFIX)pegcommon$(LIB_SUFFIX) \
    	$(LIB_DIR)/$(LIB_PREFIX)pegrepository$(LIB_SUFFIX) \
	$(LIB_DIR)/$(LIB_PREFIX)pegcompiler$(LIB_SUFFIX) \
	$(LIB_DIR)/$(LIB_PREFIX)peggetoopt$(LIB_SUFFIX)

ifeq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
DYNAMIC_LIBRARIES = \
       $(LIB_DIR)/$(LIB_PREFIX)pegcommon.x \
       $(LIB_DIR)/$(LIB_PREFIX)pegrepository.x \
       $(LIB_DIR)/$(LIB_PREFIX)pegcompiler.x \
       $(LIB_DIR)/$(LIB_PREFIX)peggetoopt.x
else
DYNAMIC_LIBRARIES = \
	-lpegcommon \
    	-lpegrepository \
	-lpegcompiler \
	-lpeggetoopt
endif

#EXTRA_INCLUDES = $(SYS_INCLUDES)
EXTRA_INCLUDES = $(SYS_INCLUDES)

PROGRAM = cimmof

SOURCES = main.cpp cmdline.cpp cmdlineExceptions.cpp $(EXTRA_SOURCES)

include $(ROOT)/mak/program.mak


tests:
