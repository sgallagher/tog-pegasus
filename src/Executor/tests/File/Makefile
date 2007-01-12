ROOT = $(PEGASUS_ROOT)
DIR = Executor/tests/File

include $(ROOT)/mak/config.mak

PROGRAM = TestExecutorFile
SOURCES = TestExecutorFile.c
EXTRA_INCLUDES += -I../../..

LIBRARIES += pegexecutor

include ../common.mak
include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:
