ROOT = ../../../../../..

DIR = Pegasus/Security/UserManager/tests/AuthorizationHandler

include $(ROOT)/mak/config.mak

include ../libraries.mak


LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = AuthorizationHandler

SOURCES = AuthorizationHandler.cpp

include $(ROOT)/mak/program.mak

tests:
	$(MKDIRHIER) $(TMP_DIR)/repository
	$(PROGRAM)
	$(RMDIRHIER) $(TMP_DIR)/repository
	$(RM) *.passwd

poststarttests:
