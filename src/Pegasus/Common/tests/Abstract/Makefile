ROOT = $(PEGASUS_ROOT)
DIR = Pegasus/Common/tests/Abstract
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestAbstract
SOURCES = Abstract.cpp

include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:
