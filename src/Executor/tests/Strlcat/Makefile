ROOT = $(PEGASUS_ROOT)
DIR = Executor/Executor/tests/TestStrlcat

include $(ROOT)/mak/config.mak

PROGRAM = TestStrlcat
SOURCES = TestStrlcat.c
EXTRA_INCLUDES += -I../../..

include ../common.mak
include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:
