ROOT = $(PEGASUS_ROOT)
DIR = Executor/tests/Socket

include $(ROOT)/mak/config.mak

PROGRAM = TestExecutorSocket
SOURCES = TestExecutorSocket.c
EXTRA_INCLUDES += -I../../..

LIBRARIES += pegexecutor

include ../common.mak
include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:
