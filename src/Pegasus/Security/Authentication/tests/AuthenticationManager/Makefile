ROOT = ../../../../../..

DIR = Pegasus/Security/Authentication/tests/AuthenticationManager

include $(ROOT)/mak/config.mak

include ../libraries.mak


LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestAuthenticationManager

SOURCES = AuthenticationManager.cpp

include $(ROOT)/mak/program.mak

tests:
	$(MKDIRHIER) $(TMP_DIR)/repository
	$(PROGRAM)
	$(RMDIRHIER) $(TMP_DIR)/repository

poststarttests:

