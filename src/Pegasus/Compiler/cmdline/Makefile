ROOT = ../../../..
DIR = Pegasus/Compiler/cmdline
include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_CMDLINE_INTERNAL

LIBRARIES = \
	pegcommon \
    	pegrepository \
	pegcompiler \
	peggetoopt

EXTRA_INCLUDES = $(SYS_INCLUDES)

PROGRAM = cimmof

SOURCES = main.cpp cmdline.cpp cmdlineExceptions.cpp $(EXTRA_SOURCES)

include $(ROOT)/mak/program.mak


tests:
