ROOT = ../../../../..
DIR = Pegasus/Provider/tests/LoadProcessClass
include $(ROOT)/mak/config.mak
include ../libraries.mak

EXTRA_INCLUDES = $(SYS_INCLUDES) $(ACE_INCLUDES)

PROGRAM = LoadProcessClass

SOURCES = LoadProcessClass.cpp

include $(ROOT)/mak/program.mak

tests:

load:
	@ LoadProcessClass $(REPOSITORY_ROOT)
