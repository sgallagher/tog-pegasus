ROOT = ../..
DIR = Server
include $(ROOT)/mak/config.mak

LIBRARIES = \
    $(LIB_DIR)/$(LIB_PREFIX)pegcommon$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegrepository$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegprotocol$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegserver$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegprovider$(LIB_SUFFIX)

EXTRA_INCLUDES = $(SYS_INCLUDES)

PROGRAM = cimserver

SOURCES = cimserver.cpp

include $(ROOT)/mak/program.mak
