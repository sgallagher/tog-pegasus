ROOT = ../../../../..
DIR = Pegasus/CQL/tests/QueryContext

#DIRS = msg 

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

LIBRARIES = \
   pegcommon \
   pegprovider \
   pegrepository \
   pegcql

PROGRAM = TestQueryContext
SOURCES = TestQueryContext.cpp

include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:
install:
