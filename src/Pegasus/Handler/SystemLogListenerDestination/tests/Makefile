ROOT = ../../../../..
DIR = Pegasus/Handler/SystemLogListenerDestination/tests
include $(ROOT)/mak/config.mak
include ../../libraries.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestSystemLogHandler

SOURCES = SystemLogHandler.cpp

include $(ROOT)/mak/program.mak

tests:

poststarttests:
	@$(PROGRAM)

install:
