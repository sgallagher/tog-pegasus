ROOT = ../../../../..
DIR = Pegasus/Common/tests/Array
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestArray

SOURCES = Array.cpp Str.cpp Int.cpp

include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:
