ROOT = ../../../../..
DIR = Pegasus/Common/tests/Reference
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestReference
SOURCES = Reference.cpp

include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:
