ROOT = $(PEGASUS_ROOT)
DIR = Executor/tests/Config

include $(ROOT)/mak/config.mak

PROGRAM = TestExecutorConfig
SOURCES = TestExecutorConfig.c
EXTRA_INCLUDES += -I../../..

LIBRARIES += pegexecutor

include ../common.mak
include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:
