ROOT = ../../../../../..
DIR = Pegasus/Common/tests/AnonymousPipe/Parent
include $(ROOT)/mak/config.mak

LIBRARIES = \
    pegcommon

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestAnonymousPipe

SOURCES = AnonymousPipe.cpp

include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:
install:
