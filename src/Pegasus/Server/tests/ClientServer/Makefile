ROOT = ../../../../..
DIR = Pegasus/Server/tests/ClientServer
include $(ROOT)/mak/config.mak
include ../libraries.mak

LIBRARIES += pegclient

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = ClientServer

SOURCES = ClientServer.cpp

include $(ROOT)/mak/program.mak

tests:
