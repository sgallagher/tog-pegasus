ROOT = ../../../../..
DIR = Pegasus/CQL/tests/CQLChainedIdentifier

#DIRS = msg 

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestCQLChainedIdentifier

LIBRARIES = \
    pegprm \
    pegprovider \
    pegrepository \
    pegconfig \
    pegclient \
    pegcql \
    pegquerycommon \
    pegcommon

SOURCES = CQLChainedIdentifier.cpp


include $(ROOT)/mak/program.mak

tests: 
	$(PROGRAM)

poststarttests:
install:
