ROOT = ../../../../..
DIR = Pegasus/Common/tests/Method
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestMethod
SOURCES = Method.cpp

include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:
