ROOT = ../../../../..
DIR = Pegasus/CQL/tests/QueryContext

#DIRS = msg 

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

LIBRARIES = \
    pegprovider \
    pegrepository \
    pegconfig \
    pegclient \
    pegcql \
    pegquerycommon \
    pegcommon

PROGRAM = TestQueryContext
SOURCES = TestQueryContext.cpp

include $(ROOT)/mak/program.mak

tests:

poststarttests:
	$(PROGRAM)

install:
