ROOT = ../../../../../..

DIR = Pegasus/Security/UserManager/tests/AuthorizationHandler

include $(ROOT)/mak/config.mak

include ../libraries.mak


LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = AuthorizationHandler

SOURCES = AuthorizationHandler.cpp

include $(ROOT)/mak/program.mak

tests:
	$(MKDIRHIER) ./repository
	$(PROGRAM)
	$(RMDIRHIER) ./repository
	$(RM) *.passwd

poststarttests:
