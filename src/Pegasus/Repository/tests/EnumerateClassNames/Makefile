ROOT = ../../../../..
DIR = Pegasus/Repository/tests/EnumerateClassNames
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = EnumerateClassNames
SOURCES = EnumerateClassNames.cpp

include $(ROOT)/mak/program.mak

tests:
	$(RMDIRHIER) repository
	$(PROGRAM)
	$(RMDIRHIER) repository

poststarttests:

