ROOT = ../../../../..
DIR = Pegasus/IndicationService/tests/DisableEnable
include $(ROOT)/mak/config.mak
include ../libraries.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY 

PROGRAM = TestDisableEnable

SOURCES = DisableEnable.cpp

include $(ROOT)/mak/program.mak

tests:

poststarttests:
	@$(ECHO) "TestDisableEnable setup"
	@$(PROGRAM) setup
	@$(ECHO) "TestDisableEnable create"
	@$(PROGRAM) create
	cimprovider -d -m ProcessIndicationProviderModule
	@$(ECHO) "TestDisableEnable delete"
	@$(PROGRAM) delete
	cimprovider -e -m ProcessIndicationProviderModule
	@$(ECHO) "TestDisableEnable create"
	@$(PROGRAM) create
	cimprovider -d -m ProcessIndicationProviderModule
	cimprovider -e -m ProcessIndicationProviderModule
	@$(ECHO) "TestDisableEnable delete"
	@$(PROGRAM) delete
	@$(ECHO) "TestDisableEnable create"
	@$(PROGRAM) create
	@$(ECHO) "TestDisableEnable create2"
	@$(PROGRAM) create2
	@$(ECHO) "TestDisableEnable addProvider"
	@$(PROGRAM) addProvider
	@$(ECHO) "TestDisableEnable modifyProvider1"
	@$(PROGRAM) modifyProvider1
	@$(ECHO) "TestDisableEnable modifyProvider2"
	@$(PROGRAM) modifyProvider2
	@$(ECHO) "TestDisableEnable deleteProvider"
	@$(PROGRAM) deleteProvider
	@$(ECHO) "TestDisableEnable delete2"
	@$(PROGRAM) delete2
	@$(ECHO) "TestDisableEnable delete"
	@$(PROGRAM) delete
	@$(ECHO) "TestDisableEnable cleanup"
	@$(PROGRAM) cleanup
	@$(ECHO) "+++++ TestDisableEnable passed all tests"

install:
