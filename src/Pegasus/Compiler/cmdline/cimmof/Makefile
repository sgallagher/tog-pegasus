ROOT = ../../../../..
DIR = Pegasus/Compiler/cmdline/cimmof
include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_CMDLINE_INTERNAL -DPEGASUS_INTERNALONLY

LIBRARIES = \
	pegcommon \
        pegrepository \
	pegcompiler \
	peggetoopt

EXTRA_INCLUDES = $(SYS_INCLUDES)

PROGRAM = cimmof

SOURCES = main.cpp cmdline.cpp cmdlineExceptions.cpp $(EXTRA_SOURCES)

include $(ROOT)/mak/program.mak

messages:

tests:

poststarttests: 
