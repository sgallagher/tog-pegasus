ROOT = ../../../../..
DIR = Pegasus/CQL/tests/CQLValue

#DIRS = msg 

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestCQLValue

LIBRARIES = \
    pegprm \
    pegprovider \
    pegrepository \
    pegconfig \
    pegclient \
    pegcql \
    pegquerycommon \
    pegcommon

SOURCES = CQLValue.cpp


include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:
install:
