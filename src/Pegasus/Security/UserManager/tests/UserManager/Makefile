ROOT = ../../../../../..

DIR = Pegasus/Security/UserManager/tests/UserManager

include $(ROOT)/mak/config.mak

include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = UserManagerTest

SOURCES = UserManager.cpp

include $(ROOT)/mak/program.mak

tests:
	$(MKDIRHIER) ./repository
	$(PROGRAM)
	$(RMDIRHIER) ./repository
	$(RM) *.passwd

poststarttests:
