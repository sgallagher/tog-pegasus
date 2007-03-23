ROOT = $(PEGASUS_ROOT)
DIR = Executor/tests/Random

include $(ROOT)/mak/config.mak

PROGRAM = TestExecutorRandom
SOURCES = TestExecutorRandom.c
EXTRA_INCLUDES += -I../../..

LIBRARIES += pegexecutor

include ../common.mak
include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:
