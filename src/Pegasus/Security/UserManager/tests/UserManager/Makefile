ROOT = ../../../../../..

DIR = Pegasus/Security/UserManager/tests/UserManager

include $(ROOT)/mak/config.mak

include ../libraries.mak

PROGRAM = UserManagerTest

SOURCES = UserManager.cpp

include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM)
	$(RM) *.passwd

poststarttests:
