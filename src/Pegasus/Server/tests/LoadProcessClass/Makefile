ROOT = ../../../../..
DIR = Pegasus/Server/tests/LoadProcessClass
include $(ROOT)/mak/config.mak
include ../libraries.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = LoadProcessClass

SOURCES = LoadProcessClass.cpp

include $(ROOT)/mak/program.mak

tests:
	LoadProcessClass $(REPOSITORY_ROOT)

poststarttests:

load:
	LoadProcessClass $(REPOSITORY_ROOT)
