ROOT = $(PEGASUS_ROOT)
DIR = Executor/tests/Messages

include $(ROOT)/mak/config.mak

PROGRAM = TestExecutorMessages
SOURCES = TestExecutorMessages.c
EXTRA_INCLUDES += -I../../..

LIBRARIES += pegexecutor

include ../common.mak
include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:
