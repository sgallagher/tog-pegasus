ROOT = ../../../../..
DIR = Pegasus/Server/tests/ClientServer
include $(ROOT)/mak/config.mak
include ../libraries.mak

LIBRARIES += $(LIB_DIR)/$(LIB_PREFIX)pegclient$(LIB_SUFFIX)

EXTRA_INCLUDES = $(SYS_INCLUDES)

PROGRAM = ClientServer

SOURCES = ClientServer.cpp

include $(ROOT)/mak/program.mak

tests:
	ClientServer
