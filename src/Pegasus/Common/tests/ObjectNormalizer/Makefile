ROOT = ../../../../..
DIR = Pegasus/Common/tests/ObjectNormalizer

include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestObjectNormalizer

SOURCES = TestObjectNormalizer.cpp

LIBRARIES += pegrepository

include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:
install:
