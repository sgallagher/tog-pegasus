ROOT = ../../../../..
DIR = Pegasus/Common/tests/MessageQueue
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestMessageQueue
SOURCES = MessageQueue.cpp

include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM)
