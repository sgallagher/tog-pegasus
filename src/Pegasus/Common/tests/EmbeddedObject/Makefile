ROOT = $(PEGASUS_ROOT)
DIR = Pegasus/Common/tests/EmbeddedObject
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestEmbeddedObject
SOURCES = EmbeddedObject.cpp

include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:

