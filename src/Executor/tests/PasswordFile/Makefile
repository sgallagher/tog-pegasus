ROOT = $(PEGASUS_ROOT)
DIR = Executor/tests/PasswordFile

include $(ROOT)/mak/config.mak

PROGRAM = TestExecutorPasswordFile
SOURCES = TestExecutorPasswordFile.c
EXTRA_INCLUDES += -I../../..

LIBRARIES += pegexecutor

include ../common.mak
include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:
