ROOT = ../../../../..
DIR = Pegasus/Common/tests/MessageSerializer
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestMessageSerializer
SOURCES = MessageSerializer.cpp

include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:
install:
