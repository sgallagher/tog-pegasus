ROOT = ../../../../..
DIR = Pegasus/Server/tests/ClientServer
include $(ROOT)/mak/config.mak
include ../libraries.mak

LIBRARIES += $(LIB_DIR)/$(LIB_PREFIX)pegclient$(LIB_SUFFIX)

DYNAMIC_LIBRARIES += -lpegclient

EXTRA_INCLUDES = $(SYS_INCLUDES)

PROGRAM = ClientServer

SOURCES = ClientServer.cpp

include $(ROOT)/mak/program.mak

tests:
