ROOT = ../../../../../..

DIR = Pegasus/Security/Authentication/tests/LocalAuthenticationHandler

include $(ROOT)/mak/config.mak

include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestLocalAuthenticationHandler

SOURCES = LocalAuthenticationHandler.cpp

include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:

