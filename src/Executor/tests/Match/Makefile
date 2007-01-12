ROOT = $(PEGASUS_ROOT)
DIR = Executor/tests/Match

include $(ROOT)/mak/config.mak

PROGRAM = TestExecutorMatch
SOURCES = TestExecutorMatch.c
EXTRA_INCLUDES += -I../../..

LIBRARIES += pegexecutor

include ../common.mak
include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:
