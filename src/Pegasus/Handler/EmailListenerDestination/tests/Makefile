ROOT = ../../../../..
DIR = Pegasus/Handler/EmailListenerDestination/tests
include $(ROOT)/mak/config.mak
include ../../libraries.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestEmailHandler

SOURCES = EmailHandler.cpp

include $(ROOT)/mak/program.mak

tests:

poststarttests:
	@$(PROGRAM)

install:
