ROOT = ../../../../..
DIR = Pegasus/Common/tests/MessageQueueService
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = MessageQueueService
SOURCES = MessageQueueService.cpp

include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:

