ROOT = ../../../../..
DIR = Pegasus/Server/tests/LoadProcessClass
include $(ROOT)/mak/config.mak
include ../libraries.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

PROGRAM = LoadProcessClass

SOURCES = LoadProcessClass.cpp

include $(ROOT)/mak/program.mak

tests:
	LoadProcessClass $(REPOSITORY_ROOT)

load:
	LoadProcessClass $(REPOSITORY_ROOT)
