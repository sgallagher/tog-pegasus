ROOT = ../../../../../..

DIR = Pegasus/Security/Authentication/tests/LocalAuthFile

include $(ROOT)/mak/config.mak

include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestLocalAuthFile

SOURCES = LocalAuthFile.cpp

include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:

