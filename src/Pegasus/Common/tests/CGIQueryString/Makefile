ROOT = ../../../../..
DIR = Pegasus/Common/tests/CGIQueryString
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestCGIQueryString
SOURCES = CGIQueryString.cpp

include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:
