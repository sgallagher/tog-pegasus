ROOT = ../../../../..
DIR = Pegasus/CQL/tests/CQLChainedIdentifier

#DIRS = msg 

include $(ROOT)/mak/config.mak
#include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestCQLChainedIdentifier

LIBRARIES = \
   pegcommon \
   pegquerycommon \
   pegcql


SOURCES = CQLChainedIdentifier.cpp


include $(ROOT)/mak/program.mak

tests: 
	$(PROGRAM)

poststarttests:
install:
