ROOT = ../../../../..
DIR = Pegasus/CQL/tests/CQLValue

#DIRS = msg 

include $(ROOT)/mak/config.mak
#include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestCQLValue

LIBRARIES = \
   pegcommon \
   pegprovider \
   pegrepository \
   pegcql


SOURCES = CQLValue.cpp


include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:
install:
