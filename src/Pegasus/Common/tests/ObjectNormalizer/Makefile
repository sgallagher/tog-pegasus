ROOT = ../../../../..
DIR = Pegasus/Common/tests/Resolve2
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestObjectNormalizer

SOURCES = ObjectNormalizer.cpp

LIBRARIES += pegrepository

include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:
install:
