ROOT = ../../../../..
DIR = Pegasus/CQL/tests/RegularExpression
#DIRS = msg
include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

LIBRARIES = \
    pegrepository \
    pegconfig \
    pegcql \
    pegquerycommon \
    pegcommon

PROGRAM = RegularExpression
SOURCES = RegularExpression.cpp

include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM) 

poststarttests: 

