ROOT = ../../../../../..

DIR = Pegasus/Security/Authentication/tests/AuthenticationManager

include $(ROOT)/mak/config.mak

include ../libraries.mak


PROGRAM = TestAuthenticationManager

SOURCES = AuthenticationManager.cpp

include $(ROOT)/mak/program.mak

tests:
	$(MKDIRHIER) ./repository
	$(PROGRAM)
	$(RMDIRHIER) ./repository

poststarttests:

