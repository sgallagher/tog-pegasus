ROOT = ../../../../..
DIR = Pegasus/Common/tests/Object
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestObject
SOURCES = Object.cpp

include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:
