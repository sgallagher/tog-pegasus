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
	@echo "TestDisableEnable setup"
	@$(PROGRAM) setup
	@echo "TestDisableEnable create"
	@$(PROGRAM) create
	cimprovider -d -m ProcessIndicationProviderModule
	@echo "TestDisableEnable delete"
	@$(PROGRAM) delete
	cimprovider -e -m ProcessIndicationProviderModule
	@echo "TestDisableEnable create"
	@$(PROGRAM) create
	cimprovider -d -m ProcessIndicationProviderModule
	cimprovider -e -m ProcessIndicationProviderModule
	@echo "TestDisableEnable delete"
	@$(PROGRAM) delete
	@echo "TestDisableEnable cleanup"
	@$(PROGRAM) cleanup
	@echo "+++++ TestDisableEnable passed all tests"
