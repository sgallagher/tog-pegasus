ROOT = ../../../../..
DIR = Pegasus/Server/tests/Server
include $(ROOT)/mak/config.mak
include ../libraries.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

PROGRAM = Server

SOURCES = Server.cpp

include $(ROOT)/mak/program.mak

tests:

run:
	Server $(REPOSITORY_ROOT)
