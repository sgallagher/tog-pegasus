ROOT = ../../..

DIR = Pegasus/Clients/cimconfig

include $(ROOT)/mak/config.mak

LIBRARIES = \
    $(LIB_DIR)/$(LIB_PREFIX)pegcommon$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegconfig$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)peggetoopt$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegclient$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegcliutils$(LIB_SUFFIX)

DYNAMIC_LIBRARIES += -lpegcommon \
-lpegconfig \
-lpeggetoopt \
-lpegclient \
-lpegcliutils

EXTRA_INCLUDES = $(SYS_INCLUDES)

PROGRAM = cimconfig

SOURCES = CIMConfigCommand.cpp

include $(ROOT)/mak/program.mak

tests:

install:
