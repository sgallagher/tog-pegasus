ROOT = ../../../../..
DIR = Pegasus/Common/tests/Compare
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestCompare
SOURCES = Compare.cpp

include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:
