ROOT = ../../../../..
DIR = Pegasus/Common/tests/HashTable
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestHashTable
SOURCES = HashTable.cpp

include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests: 
