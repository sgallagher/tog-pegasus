ROOT = ../../../../..
DIR = Pegasus/Listener/tests/TestListener
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY -DPEGASUS_CONSUMER_INTERNAL

PROGRAM = TestListener
SOURCES = TestListener.cpp

include $(ROOT)/mak/program.mak

tests:

poststarttests:

run:
	$(PROGRAM)
