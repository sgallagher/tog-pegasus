ROOT = ../../../../..
DIR = Pegasus/Common/tests/HTTPListener
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestHTTPListener
SOURCES = HTTPListener.cpp

include $(ROOT)/mak/program.mak

tests:
	TestHTTPListener
